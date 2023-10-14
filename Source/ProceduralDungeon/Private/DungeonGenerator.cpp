/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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
#include "ProceduralLevelStreaming.h"
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
	NetUpdateFrequency = 2;

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
		bGenerate = true;
		// TODO: wake up here for dormancy
	}
}

void ADungeonGenerator::CreateDungeon()
{
	// Only server generate the dungeon
	// DungeonGraph will be replicated to all clients
	if (!HasAuthority())
		return;

	int TriesLeft = MaxTry;
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
			LogError("GenerationType value is not supported.");
			return;
		}

		URoomData* def = ChooseFirstRoomData();
		if (!IsValid(def))
		{
			LogError("ChooseFirstRoomData returned null.");
			continue;
		}

		URoom* root = NewObject<URoom>(this);
		root->Init(def, this, 0);
		Graph->AddRoom(root);

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
		LogError(FString::Printf(TEXT("Generated dungeon is not valid after %d tries. Make sure your IsValidDungeon function is correct."), MaxTry));
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
			TSubclassOf<ADoor> DoorClass = ChooseDoor(Room->GetRoomData(), nullptr != r ? r->GetRoomData() : nullptr, Room->GetRoomData()->Doors[i].Type);

			if (nullptr != DoorClass)
			{
				FVector InstanceDoorPos = GetDungeonRotation().RotateVector(FDoorDef::GetRealDoorPosition(DoorCell, DoorRot)) + GetDungeonOffset();
				FQuat InstanceDoorRot = GetDungeonRotation() * FRotator(0, -90 * (int8)DoorRot, 0).Quaternion();
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
					LogError("Failed to spawn Door, make sure you set door actor to always spawning.");
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
		LogError(FString::Printf(TEXT("The room data '%s' has no door! Nothing could be generated with it!"), *GetNameSafe(ParentRoom.GetRoomData())));

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

		int nbTries = MaxRoomTry;
		URoom* newRoom = nullptr;
		// Try to place a new room
		do
		{
			nbTries--;
			URoomData* roomDef = ChooseNextRoomData(ParentRoom.GetRoomData(), doorDef);
			if (!IsValid(roomDef))
			{
				LogError("ChooseNextRoomData returned null.");
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
				LogError("ChooseNextRoomData returned a room with no compatible door.");
				continue;
			}

			// Create room from roomdef and set connections with current room
			newRoom = NewObject<URoom>(this);
			newRoom->Init(roomDef, this, InOutRoomList.Num());

			int doorIndex = compatibleDoors[(roomDef->RandomDoor && compatibleDoors.Num() > 1) ? Random.RandRange(0, compatibleDoors.Num() - 1) : 0];

			// Place the room at its new position with the correct rotation
			newRoom->SetPositionAndRotationFromDoor(doorIndex, newRoomPos, newRoomDoorDir);

			// Test if it fits in the place
			if (!URoom::Overlap(*newRoom, InOutRoomList))
			{
				// connect the doors to all possible existing rooms
				URoom::Connect(*newRoom, doorIndex, ParentRoom, i);
				if (Dungeon::CanLoop())
				{
					newRoom->TryConnectToExistingDoors(InOutRoomList);
				}
				InOutRoomList.Add(newRoom);
				AddedRooms.Add(newRoom);
				OnRoomAdded(newRoom->GetRoomData());
			}
			else
			{
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
		Room->Destroy(GetWorld());
	}
}

void ADungeonGenerator::UpdateRoomVisibility()
{
	if (!Dungeon::OcclusionCulling())
		return;

	APawn* Player = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawnOrSpectator();
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

	uint32 OcclusionDistance = Dungeon::OcclusionDistance();
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

	LogInfo(FString::Printf(TEXT("Seed: %d"), Seed));
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
	switch (State)
	{
	case EGenerationState::Unload:
		LogInfo("======= Begin Unload All Levels =======");
		Reset();
		LogInfo(FString::Printf(TEXT("Nb Room To Unload: %d"), Graph->Count()));
		UnloadAllRooms();
		break;
	case EGenerationState::Generation:
		LogInfo("======= Begin Dungeon Generation =======");
		++Generation;
		bGenerate = false;
		UpdateSeed();
		CreateDungeon();
	case EGenerationState::Initialization:
		LogInfo("======= Begin Dungeon Initialization =======");
		Graph->SynchronizeRooms();
		UpdateOctree();
		break;
	case EGenerationState::Load:
		LogInfo("======= Begin Load All Levels =======");
		LogInfo(FString::Printf(TEXT("Nb Room To Load: %d"), Graph->Count()));
		LoadAllRooms();
		break;
	case EGenerationState::Idle:
		LogInfo("======= Ready To Play =======");
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
		if (Graph->IsDirty() || bGenerate)
			SetState(EGenerationState::Unload);
		break;
	case EGenerationState::Unload:
		if (Graph->AreRoomsUnloaded())
			SetState((HasAuthority() && bGenerate) ? EGenerationState::Generation : EGenerationState::Initialization);
		break;
	case EGenerationState::Generation:
		SetState(EGenerationState::Initialization);
		break;
	case EGenerationState::Initialization:
		SetState(EGenerationState::Load);
		break;
	case EGenerationState::Load:
		if (Graph->AreRoomsInitialized())
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
		LogInfo("======= End Unload All Levels =======");
		break;
	case EGenerationState::Generation:
		LogInfo("======= End Dungeon Generation =======");
		break;
	case EGenerationState::Initialization:
		LogInfo("======= End Dungeon Initialization =======");
		break;
	case EGenerationState::Load:
		LogInfo("======= End Load All Levels =======");

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
		{
			LogInfo("Rebuild navmesh");
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
	LogError("Error: ChooseFirstRoomData not implemented");
	return nullptr;
}

URoomData* ADungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const FDoorDef& DoorData)
{
	LogError("Error: ChooseNextRoomData not implemented");
	return nullptr;
}

TSubclassOf<ADoor> ADungeonGenerator::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType)
{
	LogError("Error: ChooseDoor not implemented");
	return nullptr;
}

bool ADungeonGenerator::IsValidDungeon_Implementation()
{
	LogError("Error: IsValidDungeon not implemented");
	return false;
}

bool ADungeonGenerator::ContinueToAddRoom_Implementation()
{
	LogError("Error: ContinueToAddRoom not implemented");
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
