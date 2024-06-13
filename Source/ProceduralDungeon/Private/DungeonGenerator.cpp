/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "DungeonGenerator.h"
#include "Engine/World.h"
#include "Engine/Engine.h" // GEngine
#include "Net/UnrealNetwork.h" // DOREPLIFETIME
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "RoomData.h"
#include "Room.h"
#include "Door.h"
#include "RoomLevel.h"
#include "ProceduralDungeon.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "QueueOrStack.h"
#include "DungeonGraph.h"
#include <functional>
#include <EngineUtils.h>

uint32 ADungeonGenerator::GeneratorCount = 0;

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GenerationType = EGenerationType::DFS;
	SeedType = ESeedType::Random;
	Seed = 123456789; // default Seed
	SeedIncrement = 123456; // default Seed increment
	UniqueId = GeneratorCount++; // TODO: make it better than a static increment. It can be increased very quickly in editor when we move an actor.
	bUseGeneratorTransform = false;

	bAlwaysRelevant = true;
	bReplicates = true;
	NetPriority = 10.0f;
	NetUpdateFrequency = 20;
	NetDormancy = ENetDormancy::DORM_DormantAll;

	Graph = CreateDefaultSubobject<UDungeonGraph>(TEXT("Dungeon Rooms"));
	Octree = MakeUnique<FDungeonOctree>(FVector::ZeroVector, HALF_WORLD_MAX);
}

void ADungeonGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADungeonGenerator, Seed);
	DOREPLIFETIME(ADungeonGenerator, Generation);
	DOREPLIFETIME(ADungeonGenerator, Graph);
}

bool ADungeonGenerator::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	bWroteSomething |= Graph->ReplicateSubobject(Channel, Bunch, RepFlags);
	return bWroteSomething;
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	Graph->Generator = this;
}

void ADungeonGenerator::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(EndPlayReason == EEndPlayReason::Destroyed)
		UnloadAllRooms();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OnStateTick(CurrentState);
}

void ADungeonGenerator::Generate()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
	{
		Graph->RequestGeneration();
		// TODO: wake up here for dormancy
	}
}

void ADungeonGenerator::Unload()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
	{
		Graph->RequestUnload();
		// TODO: wake up here for dormancy
	}
}

void ADungeonGenerator::CreateDungeon()
{
	// Only server generate the dungeon
	// DungeonGraph will be replicated to all clients
	if (!HasAuthority())
		return;

	int TriesLeft = Dungeon::MaxGenerationTryBeforeGivingUp();
	bool ValidDungeon = false;

	// generate level until IsValidDungeon return true
	do {
		TriesLeft--;

		// Reset generation data
		OnGenerationInit();

		// Create the first room
		Graph->Clear();

		// Create the list with the correct mode (depth or breadth)
		TQueueOrStack<URoom*>::EMode listMode;
		switch (GenerationType)
		{
		case EGenerationType::DFS:
			listMode = TQueueOrStack<URoom*>::EMode::STACK;
			break;
		case EGenerationType::BFS:
			listMode = TQueueOrStack<URoom*>::EMode::QUEUE;
			break;
		default:
			DungeonLog_Error("GenerationType value is not supported.");
			return;
		}

		URoomData* def = ChooseFirstRoomData();
		if (!IsValid(def))
		{
			DungeonLog_Error("ChooseFirstRoomData returned null.");
			continue;
		}

		URoom* root = NewObject<URoom>(this);
		root->Init(def, this, 0);
		Graph->AddRoom(root);
		OnRoomAdded(root->GetRoomData());

		// Build the list of rooms
		TQueueOrStack<URoom*> roomStack(listMode);
		roomStack.Push(root);
		URoom* currentRoom = nullptr;
		TArray<URoom*> newRooms;
		while (!roomStack.IsEmpty())
		{
			currentRoom = roomStack.Pop();
			check(IsValid(currentRoom)); // currentRoom should always be valid

			if (!AddNewRooms(*currentRoom, newRooms, Graph->Rooms))
				break;

			for (URoom* room : newRooms)
			{
				roomStack.Push(room);
			}
		}

		// Initialize the dungeon by eg. altering the room instances
		Graph->InitRooms();
		InitializeDungeon(Graph);

		ValidDungeon = IsValidDungeon();
	} while (TriesLeft > 0 && !ValidDungeon);

	if (!ValidDungeon)
	{
		DungeonLog_Error("Generated dungeon is not valid after %d tries. Make sure your IsValidDungeon function is correct.", Dungeon::MaxGenerationTryBeforeGivingUp());
		Graph->Clear();
		OnGenerationFailed();
		return;
	}

	return;
}

void ADungeonGenerator::InstantiateRoom(URoom* Room)
{
	// Instantiate room
	Room->Instantiate(GetWorld());

	// Spawn only doors on server
	if (!HasAuthority())
		return;

	for (int i = 0; i < Room->GetConnectionCount(); i++)
	{
		// Get next room
		URoom* r = Room->GetConnection(i).Get();
		FIntVector DoorCell = Room->GetDoorWorldPosition(i);
		EDoorDirection DoorRot = Room->GetDoorWorldOrientation(i);
		int j = Room->GetOtherDoorIndex(i);

		// Don't instantiate door if it's the parent
		if (!Room->IsDoorInstanced(i))
		{
			bool Flipped = false;
			TSubclassOf<ADoor> DoorClass = ChooseDoor(Room->GetRoomData(), nullptr != r ? r->GetRoomData() : nullptr, Room->GetRoomData()->Doors[i].Type, Flipped);

			if (nullptr != DoorClass)
			{
				FVector InstanceDoorPos = GetDungeonRotation().RotateVector(FDoorDef::GetRealDoorPosition(DoorCell, DoorRot)) + GetDungeonOffset();
				FQuat InstanceDoorRot = GetDungeonRotation() * FRotator(0, 90 * (int8)DoorRot + Flipped * 180, 0).Quaternion();
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot.Rotator(), SpawnParams);

				if (IsValid(Door))
				{
					DoorList.Add(Door);
					Door->SetConnectingRooms(Room, r);
					Room->SetDoorInstance(i, Door);
					if (IsValid(r))
					{
						r->SetDoorInstance(j, Door);
					}
				}
				else
				{
					DungeonLog_Error("Failed to spawn Door, make sure you set door actor to always spawning.");
				}
			}
		}
	}
}

bool ADungeonGenerator::AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms, TArray<URoom*>& InOutRoomList)
{
	check(HasAuthority());

	int nbDoor = ParentRoom.GetRoomData()->GetNbDoor();
	if (nbDoor <= 0)
		DungeonLog_Error("The room data '%s' has no door! Nothing could be generated with it!", *GetNameSafe(ParentRoom.GetRoomData()));

	// Cache world before loops
	const UWorld* World = GetWorld();
	const FTransform DungeonTransform = bUseGeneratorTransform ? GetTransform() : FTransform::Identity;

	// Collision params to use if bUseWorldCollisionChecks is true
	FCollisionQueryParams Params;
	Params.bIgnoreTouches = true;
	Params.AddIgnoredActor(this);

	AddedRooms.Reset();
	bool shouldContinue = false;
	for (int i = 0; shouldContinue = ContinueToAddRoom(), i < nbDoor && shouldContinue; ++i)
	{
		if (ParentRoom.IsConnected(i))
			continue;

		// Get the door definition in its world position and direction
		FDoorDef doorDef = ParentRoom.GetRoomData()->Doors[i];
		doorDef.Position = ParentRoom.RoomToWorld(doorDef.Position);
		doorDef.Direction = ParentRoom.RoomToWorld(doorDef.Direction);
		const FIntVector newRoomPos = doorDef.Position + ToIntVector(doorDef.Direction);
		const EDoorDirection newRoomDoorDir = ~doorDef.Direction;

		int nbTries = Dungeon::MaxRoomPlacementTryBeforeGivingUp();
		URoom* newRoom = nullptr;
		// Try to place a new room
		do
		{
			nbTries--;
			int doorIndex = -1;
			URoomData* roomDef = ChooseNextRoomData(ParentRoom.GetRoomData(), doorDef, doorIndex);
			if (!IsValid(roomDef))
			{
				DungeonLog_Error("ChooseNextRoomData returned null.");
				continue;
			}

			if (doorIndex >= roomDef->Doors.Num())
			{
				DungeonLog_Error("ChooseNextRoomData returned door index '%d' which is out of range in the RoomData '%s' door list (max: %d).", doorIndex, *roomDef->GetName(), roomDef->Doors.Num() - 1);
				continue;
			}

			// Get all compatible door indices from the chosen room data
			TArray<int> compatibleDoors;
			for (int k = 0; k < roomDef->GetNbDoor(); ++k)
			{
				if (FDoorDef::AreCompatible(roomDef->Doors[k], doorDef))
					compatibleDoors.Add(k);
			}

			if (compatibleDoors.Num() <= 0)
			{
				DungeonLog_Error("ChooseNextRoomData returned room data '%s' with no compatible door (door type: '%s').", *roomDef->GetName(), *doorDef.GetTypeName());
				continue;
			}

			if (roomDef->RandomDoor || (doorIndex < 0))
				doorIndex = compatibleDoors[Random.RandRange(0, compatibleDoors.Num() - 1)];
			else if (!compatibleDoors.Contains(doorIndex))
			{
				DungeonLog_Error("ChooseNextRoomData returned door index '%d' (RoomData '%s') which its type '%s' is not compatible with '%s'.", doorIndex, *roomDef->GetName(), *roomDef->Doors[doorIndex].GetTypeName(), *doorDef.GetTypeName());
				continue;
			}

			// Create room from roomdef and set connections with current room
			newRoom = NewObject<URoom>(this);
			newRoom->Init(roomDef, this, InOutRoomList.Num());
			newRoom->SetPositionAndRotationFromDoor(doorIndex, newRoomPos, newRoomDoorDir);

			if (bUseWorldCollisionChecks)
			{
				const FBoxCenterAndExtent Bounds = newRoom->GetBounds();
				const bool bCollideWithWorld = World->OverlapBlockingTestByChannel(DungeonTransform.TransformPosition(Bounds.Center), DungeonTransform.GetRotation(), ECC_WorldStatic, FCollisionShape::MakeBox(Bounds.Extent), Params);
				if (bCollideWithWorld)
				{
					// The object will be automatically deleted by the GC
					newRoom = nullptr;
					continue;
				}
			}

			// Test if it fits in the place
			if (!URoom::Overlap(*newRoom, InOutRoomList))
			{
				// connect the doors to all possible existing rooms
				URoom::Connect(*newRoom, doorIndex, ParentRoom, i);
				if (bCanLoop && Dungeon::CanLoop())
				{
					newRoom->TryConnectToExistingDoors(InOutRoomList);
				}
				InOutRoomList.Add(newRoom);
				AddedRooms.Add(newRoom);
				OnRoomAdded(newRoom->GetRoomData());
			}
			else
			{
				// The object will be automatically deleted by the GC
				newRoom = nullptr;
			}
		} while (nbTries > 0 && newRoom == nullptr);
	}

	return shouldContinue;
}

void ADungeonGenerator::LoadAllRooms()
{
	// When a level is correct, load all rooms
	for (URoom* Room : Graph->GetAllRooms())
	{
		InstantiateRoom(Room);
	}
}

void ADungeonGenerator::UnloadAllRooms()
{
	if (HasAuthority())
	{
		for (ADoor* Door : DoorList)
		{
			Door->Destroy();
		}
		DoorList.Empty();
	}

	for (URoom* Room : Graph->GetAllRooms())
	{
		check(Room);
		Room->Destroy();
	}
}

void ADungeonGenerator::UpdateRoomVisibility()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(Controller))
		return;

	APawn* Player = Controller->GetPawnOrSpectator();
	if (!IsValid(Player))
		return;

	FBox WorldPlayerBox = Player->GetComponentsBoundingBox();
	FTransform Transform = UseGeneratorTransform() ? GetTransform() : FTransform::Identity;
	WorldPlayerBox = WorldPlayerBox.InverseTransformBy(Transform);

	TSet<URoom*> RoomsToHide(CurrentPlayerRooms);
	CurrentPlayerRooms.Empty();
	FindElementsWithBoundsTest(*Octree, WorldPlayerBox, [this, &RoomsToHide](const FDungeonOctreeElement& Element)
	{
		RoomsToHide.Remove(Element.Room);
		CurrentPlayerRooms.Add(Element.Room);
		Element.Room->SetPlayerInside(true);
	});

	for (URoom* room : RoomsToHide)
	{
		room->SetPlayerInside(false);
	}

	const bool bIsOcclusionEnabled = Dungeon::OcclusionCulling();
	const uint32 OcclusionDistance = Dungeon::OcclusionDistance();

	// Detects occlusion setting changes and toggles on/off all room visibilities when occlusion is enabled/disabled.
	if (bWasOcclusionEnabled != bIsOcclusionEnabled
		|| PreviousOcclusionDistance != OcclusionDistance)
	{
		RoomsToHide.Empty();
		for (URoom* Room : Graph->GetAllRooms())
		{
			Room->SetVisible(!bIsOcclusionEnabled);
		}
	}
	bWasOcclusionEnabled = bIsOcclusionEnabled;
	PreviousOcclusionDistance = OcclusionDistance;

	// Don't change room visibilities if occlusion is disabled.
	if (!bIsOcclusionEnabled)
		return;

	TSet<URoom*> VisibleRooms;
	UDungeonGraph::TraverseRooms(CurrentPlayerRooms, &VisibleRooms, OcclusionDistance, [](URoom* room) { room->SetVisible(true); });
	UDungeonGraph::TraverseRooms(RoomsToHide, nullptr, OcclusionDistance, [&VisibleRooms](URoom* room) { room->SetVisible(VisibleRooms.Contains(room)); });
}

void ADungeonGenerator::Reset()
{
	CurrentPlayerRooms.Empty();
	Octree->Destroy();
}

void ADungeonGenerator::UpdateOctree()
{
	Octree->Destroy();
	for (URoom* r : Graph->Rooms)
	{
		check(IsValid(r));
		FBoxCenterAndExtent bounds = r->GetBounds();
		FDungeonOctreeElement octreeElement(r);
		Octree->AddElement(octreeElement);
		r->SetVisible(false);
	}
}

void ADungeonGenerator::UpdateSeed()
{
	switch (SeedType)
	{
	case ESeedType::Random:
		Random.GenerateNewSeed();
		Seed = Random.GetCurrentSeed();
		break;
	case ESeedType::AutoIncrement:
		if(bShouldIncrement)
			Seed += SeedIncrement;
		else
			bShouldIncrement = true;
		// no break so we initialize the seed too
	case ESeedType::Fixed:
		Random.Initialize(Seed);
		break;
	default:
		checkNoEntry();
		break;
	}

	DungeonLog_Info("Seed: %d", Seed);
}

/*
 *	=======================================
 *				State Machine
 *	=======================================
 */
void ADungeonGenerator::SetState(EGenerationState NewState)
{
	OnStateEnd(CurrentState);
	CurrentState = NewState;
	OnStateBegin(CurrentState);
}

void ADungeonGenerator::OnStateBegin(EGenerationState State)
{
	CachedTmpRoomCount = 0;
	switch (State)
	{
	case EGenerationState::Unload:
		DungeonLog_Info("======= Begin Unload All Levels =======");
		Reset();
		DungeonLog_Info("Nb Room To Unload: %d", Graph->Count());
		UnloadAllRooms();
		break;
	case EGenerationState::Generation:
		DungeonLog_Info("======= Begin Dungeon Generation =======");
		FlushNetDormancy();
		++Generation;
		UpdateSeed();
		CreateDungeon();
	case EGenerationState::Initialization:
		DungeonLog_Info("======= Begin Dungeon Initialization =======");
		Graph->SynchronizeRooms();
		UpdateOctree();
		break;
	case EGenerationState::Load:
		DungeonLog_Info("======= Begin Load All Levels =======");
		DungeonLog_Info("Nb Room To Load: %d", Graph->Count());
		LoadAllRooms();
		break;
	case EGenerationState::Idle:
		DungeonLog_Info("======= Ready To Play =======");
		break;
	default:
		checkNoEntry();
		break;
	}
}

void ADungeonGenerator::OnStateTick(EGenerationState State)
{
	int RoomCount = 0;
	switch (State)
	{
	case EGenerationState::Idle:
		UpdateRoomVisibility();
		if (Graph->IsDirty())
			SetState(EGenerationState::Unload);
		break;
	case EGenerationState::Unload:
		if (Graph->AreRoomsUnloaded(CachedTmpRoomCount))
			SetState((HasAuthority() && Graph->IsRequestingGeneration()) ? EGenerationState::Generation : EGenerationState::Initialization);
		break;
	case EGenerationState::Generation:
		SetState(EGenerationState::Initialization);
		break;
	case EGenerationState::Initialization:
		SetState(EGenerationState::Load);
		break;
	case EGenerationState::Load:
		if (Graph->AreRoomsInitialized(CachedTmpRoomCount))
			SetState(EGenerationState::Idle);
		break;
	default:
		break;
	}
}

void ADungeonGenerator::OnStateEnd(EGenerationState State)
{
	FTimerHandle handle;
	UNavigationSystemV1* nav = nullptr;
	switch (State)
	{
	case EGenerationState::Idle:
		OnPreGeneration();
		break;
	case EGenerationState::Unload:
		Graph->Clear();
		GetWorld()->FlushLevelStreaming();
		GEngine->ForceGarbageCollection(true);
		DungeonLog_Info("======= End Unload All Levels =======");
		break;
	case EGenerationState::Generation:
		DungeonLog_Info("======= End Dungeon Generation =======");
		break;
	case EGenerationState::Initialization:
		DungeonLog_Info("======= End Dungeon Initialization =======");
		break;
	case EGenerationState::Load:
		DungeonLog_Info("======= End Load All Levels =======");

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
		{
			DungeonLog_Info("Rebuild navmesh");
			nav->CancelBuild();
			nav->Build();
		}

		// Invoke Post Generation Event when initialization is done
		OnPostGeneration();
		break;
	default:
		break;
	}
}

// ===== Default Native Events Implementations =====

URoomData* ADungeonGenerator::ChooseFirstRoomData_Implementation()
{
	DungeonLog_Error("Error: ChooseFirstRoomData not implemented");
	return nullptr;
}

URoomData* ADungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const FDoorDef& DoorData, int& DoorIndex)
{
	DungeonLog_Error("Error: ChooseNextRoomData not implemented");
	return nullptr;
}

TSubclassOf<ADoor> ADungeonGenerator::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped)
{
	DungeonLog_Error("Error: ChooseDoor not implemented");
	return nullptr;
}

bool ADungeonGenerator::IsValidDungeon_Implementation()
{
	DungeonLog_Error("Error: IsValidDungeon not implemented");
	return false;
}

bool ADungeonGenerator::ContinueToAddRoom_Implementation()
{
	DungeonLog_Error("Error: ContinueToAddRoom not implemented");
	return false;
}

void ADungeonGenerator::InitializeDungeon_Implementation(const UDungeonGraph* Rooms)
{
}

void ADungeonGenerator::OnPreGeneration_Implementation()
{
	OnPreGenerationEvent.Broadcast();
}

void ADungeonGenerator::OnPostGeneration_Implementation()
{
	OnPostGenerationEvent.Broadcast();
}

void ADungeonGenerator::OnGenerationInit_Implementation()
{
	OnGenerationInitEvent.Broadcast();
}

void ADungeonGenerator::OnGenerationFailed_Implementation()
{
	OnGenerationFailedEvent.Broadcast();
}

void ADungeonGenerator::OnRoomAdded_Implementation(const URoomData* NewRoom)
{
	OnRoomAddedEvent.Broadcast(NewRoom);
}

// ===== Utility Functions =====

URoomData* ADungeonGenerator::GetRandomRoomData(TArray<URoomData*> RoomDataArray)
{
	if (RoomDataArray.Num() <= 0)
		return nullptr;

	int n = Random.RandRange(0, RoomDataArray.Num() - 1);
	return RoomDataArray[n];
}

URoomData* ADungeonGenerator::GetRandomRoomDataWeighted(const TMap<URoomData*, int>& RoomDataWeightedMap)
{
	if (RoomDataWeightedMap.Num() <= 0)
		return nullptr;

	int Total = Dungeon::GetTotalWeight(RoomDataWeightedMap);
	const int Chosen = Random.RandRange(0, Total - 1);
	return Dungeon::GetWeightedAt(RoomDataWeightedMap, Chosen);
}

void ADungeonGenerator::GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData)
{
	for (URoomData* RoomData : RoomDataArray)
	{
		if (RoomData->HasCompatibleDoor(DoorData))
		{
			CompatibleRooms.Add(RoomData);
			bSuccess = true;
		}
	}
}

bool ADungeonGenerator::HasAlreadyRoomData(URoomData* RoomData)
{
	return Graph->HasAlreadyRoomData(RoomData);
}

bool ADungeonGenerator::HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList)
{
	return Graph->HasAlreadyOneRoomDataFrom(RoomDataList);
}

int ADungeonGenerator::CountRoomData(URoomData* RoomData)
{
	return Graph->CountRoomData(RoomData);
}

int ADungeonGenerator::CountTotalRoomData(TArray<URoomData*> RoomDataList)
{
	return Graph->CountTotalRoomData(RoomDataList);
}

bool ADungeonGenerator::HasAlreadyRoomType(TSubclassOf<URoomData> RoomType)
{
	return Graph->HasAlreadyRoomType(RoomType);
}

bool ADungeonGenerator::HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList)
{
	return Graph->HasAlreadyOneRoomTypeFrom(RoomTypeList);
}

int ADungeonGenerator::CountRoomType(TSubclassOf<URoomData> RoomType)
{
	return Graph->CountRoomType(RoomType);
}

int ADungeonGenerator::CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList)
{
	return Graph->CountTotalRoomType(RoomTypeList);
}

int ADungeonGenerator::GetNbRoom()
{
	return Graph->Count();
}

float ADungeonGenerator::GetProgress() const
{
	const int32 TotalRoom = Graph->Count();
	switch (CurrentState)
	{
	case EGenerationState::Unload:
		return (TotalRoom > 0)
			? 0.5f * (static_cast<float>(CachedTmpRoomCount) / TotalRoom)
			: 0.0f;
	case EGenerationState::Generation:
	case EGenerationState::Initialization:
		return 0.5f;
	case EGenerationState::Load:
		return (TotalRoom > 0)
			? 0.5f + 0.5f * (static_cast<float>(CachedTmpRoomCount) / TotalRoom)
			: 0.5f;
	default:
		return 1.0f;
	}
}

URoom* ADungeonGenerator::GetRoomByIndex(int64 Index) const
{
	return Graph->GetRoomByIndex(Index);
}

void ADungeonGenerator::SetSeed(int32 NewSeed)
{
	Seed = static_cast<uint32>(NewSeed);
	bShouldIncrement = false; // avoid incrementing when SeedType is AutoIncrement
}

int32 ADungeonGenerator::GetSeed()
{
	return static_cast<int32>(Seed);
}

FVector ADungeonGenerator::GetDungeonOffset() const
{
	return UseGeneratorTransform() ? GetActorLocation() : FVector::ZeroVector;
}

FQuat ADungeonGenerator::GetDungeonRotation() const
{
	return UseGeneratorTransform() ? GetActorQuat() : FQuat::Identity;
}

// ##############################
//        CONSOLE COMMANDS
// ##############################

// An helper class for console commands
struct FDungeonConsoleCommands
{
private:
	// Fills OutGenerators array with DungeonGenerator actors patching the NameOrTag provided, or all if NameOrTag is empty.
	static bool CollectDungeonGenerators(const TCHAR* CommandName, UWorld* InWorld, const FString& NameOrTag, TArray<ADungeonGenerator*>& OutGenerators);

public:
	// Execute a Callback on all DungeonGenerator actors found depending on the InParams provided.
	static void ExecuteOnDungeonGenerators(const TCHAR* CommandName, const TArray<FString>& InParams, UWorld* InWorld, TFunction<void(ADungeonGenerator*)> Callback);

public:
	static const TCHAR* GenerateCommandName;
	static const TCHAR* UnloadCommandName;
};

const TCHAR* FDungeonConsoleCommands::GenerateCommandName = TEXT("pd.Generate");
const TCHAR* FDungeonConsoleCommands::UnloadCommandName = TEXT("pd.Unload");

FAutoConsoleCommandWithWorldAndArgs CCmdGenerateDungeon(FDungeonConsoleCommands::GenerateCommandName
	, TEXT("Call \"Generate\" on DungeonsGenerator actors with the name or tag provided, or all if nothing provided. USAGE: pd.Generate [name|tag]")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& InParams, UWorld* InWorld) {
		FDungeonConsoleCommands::ExecuteOnDungeonGenerators(FDungeonConsoleCommands::GenerateCommandName
			, InParams
			, InWorld
			, [](ADungeonGenerator* Generator) { Generator->Generate(); }
		);
		})
	, EConsoleVariableFlags::ECVF_Cheat
);

FAutoConsoleCommandWithWorldAndArgs CCmdUnloadDungeon(FDungeonConsoleCommands::UnloadCommandName
	, TEXT("Call \"Unload\" on DungeonsGenerator actors with the name or tag provided, or all if nothing provided. USAGE: pd.Unload [name|tag]")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& InParams, UWorld* InWorld) {
		FDungeonConsoleCommands::ExecuteOnDungeonGenerators(FDungeonConsoleCommands::UnloadCommandName
			, InParams
			, InWorld
			, [](ADungeonGenerator* Generator) { Generator->Unload(); }
		);
		})
	, EConsoleVariableFlags::ECVF_Cheat
);

bool FDungeonConsoleCommands::CollectDungeonGenerators(const TCHAR* CommandName, UWorld* InWorld, const FString& NameOrTag, TArray<ADungeonGenerator*>& OutGenerators)
{
	if (!IsValid(InWorld) || !InWorld->IsGameWorld())
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("[%s] Invalid world."), CommandName);
		return false;
	}

	OutGenerators.Empty();
	if (!NameOrTag.IsEmpty())
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Search for DungeonGenerator actors with name or tag '%s'."), CommandName, *NameOrTag);
		FString Name(NameOrTag);
		FName Tag(NameOrTag);
		World::FindAllActorsByPredicate<ADungeonGenerator>(InWorld, OutGenerators, [&Name, &Tag](const ADungeonGenerator* Generator) { return Generator->GetName() == Name || Generator->ActorHasTag(Tag); });
	}
	else
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Search for all DungeonGenerator actors."), CommandName);
		World::FindAllActors(InWorld, OutGenerators);
	}

	// Found no generator
	if (OutGenerators.Num() <= 0)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("[%s] No DungeonGenerator found%s.")
			, CommandName
			, (!NameOrTag.IsEmpty())
			? *FString::Printf(TEXT(" with name or tag '%s'"), *NameOrTag)
			: TEXT("")
		);
		return false;
	}

	return true;
}

void FDungeonConsoleCommands::ExecuteOnDungeonGenerators(const TCHAR* CommandName, const TArray<FString>& InParams, UWorld* InWorld, TFunction<void(ADungeonGenerator*)> Callback)
{
	UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Begin."), CommandName);

	TArray<ADungeonGenerator*> Generators;
	if (!CollectDungeonGenerators(CommandName, InWorld, (InParams.Num() > 0) ? InParams[0] : FString(), Generators))
		return;

	for (ADungeonGenerator* Generator : Generators)
	{
		if (!IsValid(Generator))
		{
			UE_LOG(LogProceduralDungeon, Warning, TEXT("[%s] Generator is invalid!."), CommandName);
			continue;
		}

		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Execute command on DungeonGenerator '%s'."), CommandName, *Generator->GetName());
		Callback(Generator);
	}
	UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] End."), CommandName);
}
