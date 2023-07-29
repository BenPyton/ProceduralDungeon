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

int32 ADungeonGenerator::GeneratorCount = 0;

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GenerationType = EGenerationType::DFS;
	SeedType = ESeedType::Random;
	Seed = 123456789; // default Seed
	UniqueId = GeneratorCount++; // TODO: make it better than a static increment. It can be increased very quickly in editor when we move an actor.
	bUseGeneratorTransform = false;

	bAlwaysRelevant = true;
	bReplicates = true;
	NetPriority = 10.0f;
	NetUpdateFrequency = 10;

	Graph = CreateDefaultSubobject<UDungeonGraph>(TEXT("Dungeon Rooms"));
	Octree = MakeUnique<FDungeonOctree>(FVector::ZeroVector, HALF_WORLD_MAX);
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
		if (SeedType == ESeedType::Random)
		{
			Random.GenerateNewSeed();
			Seed = Random.GetCurrentSeed();
		}

		BeginGeneration(Seed);

		if (SeedType == ESeedType::AutoIncrement)
		{
			Seed += 123456;
		}
	}
}

void ADungeonGenerator::BeginGeneration_Implementation(uint32 GenerationSeed)
{
	Seed = GenerationSeed;
	Random.Initialize(Seed);
	LogInfo(FString::Printf(TEXT("Seed: %d"), Seed));
	SetState(EGenerationState::Unload);
}

void ADungeonGenerator::CreateDungeon()
{
	int TriesLeft = MaxTry;
	bool ValidDungeon = false;

	// generate level until there IsValidDungeon return true
	do {
		TriesLeft--;

		// Reset generation data
		DispatchGenerationInit();

		// Create the first room
		Graph->Clear();

		URoomData* def = ChooseFirstRoomData();
		if (!IsValid(def))
		{
			LogError("ChooseFirstRoomData returned null.");
			continue;
		}

		URoom* root = NewObject<URoom>();
		root->Init(def, this, 0);
		Graph->AddRoom(root);

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

		// Build the list of rooms
		TQueueOrStack<URoom*> roomStack(listMode);
		roomStack.Push(root);
		URoom* currentRoom = nullptr;
		URoom* newRoom = nullptr;
		while (ContinueToAddRoom() && !roomStack.IsEmpty())
		{
			currentRoom = roomStack.Pop();
			check(IsValid(currentRoom)); // currentRoom should always be valid
			for (URoom* room : AddNewRooms(*currentRoom, Graph->Rooms))
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
		RoomList.Empty();
		DispatchGenerationFailed();
	}

	// Update Octree
	Octree->Destroy();
	for (URoom* r : Graph->Rooms)
	{
		check(IsValid(r))
		FBoxCenterAndExtent bounds = r->GetBounds();
		FDungeonOctreeElement octreeElement(r);
		Octree->AddElement(octreeElement);
	}
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

TArray<URoom*> ADungeonGenerator::AddNewRooms(URoom& ParentRoom, TArray<URoom*>& InOutRoomList)
{
	TArray<URoom*> newRooms;
	int nbDoor = ParentRoom.GetRoomData()->GetNbDoor();
	URoom* newRoom = nullptr;
	for (int i = 0; i < nbDoor; ++i)
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
			newRoom = NewObject<URoom>();
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
				newRooms.Add(newRoom);
				DispatchRoomAdded(newRoom->GetRoomData());
			}
			else
			{
				newRoom = nullptr;
			}
		} while (nbTries > 0 && newRoom == nullptr);
	}

	return newRooms;
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
	for (int i = 0; i < DoorList.Num(); i++)
	{
		DoorList[i]->Destroy();
	}
	DoorList.Empty();

	for (URoom* Room : Graph->GetAllRooms())
	{
		Room->Destroy(GetWorld());
	}
}

void ADungeonGenerator::UpdateRoomVisibility()
{
	if (!URoom::OcclusionCulling())
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
		DispatchPreGeneration();
		LogInfo("======= Begin Map Generation =======");
		CreateDungeon();
		break;
	case EGenerationState::Load:
		LogInfo("======= Begin Load All Levels =======");
		LogInfo(FString::Printf(TEXT("Nb Room To Load: %d"), Graph->Count()));
		LoadAllRooms();
		break;
	case EGenerationState::Initialization:
		LogInfo("======= Begin Init All Levels =======");
		LogInfo(FString::Printf(TEXT("Nb Room To Initialize: %d"), Graph->Count()));
		break;
	case EGenerationState::Play:
		LogInfo("======= Ready To Play =======");
		break;
	default:
		break;
	}
}

void ADungeonGenerator::OnStateTick(EGenerationState State)
{
	int RoomCount = 0;
	switch (State)
	{
	case EGenerationState::Unload:
		for (URoom* Room : Graph->GetAllRooms())
		{
			if (Room->IsInstanceUnloaded())
				RoomCount++;
		}

		if (RoomCount == Graph->Count())
			SetState(EGenerationState::Generation);
		break;
	case EGenerationState::Generation:
		SetState((RoomList.Num() > 0) ? EGenerationState::Load : EGenerationState::None);
		break;
	case EGenerationState::Load:
		for (URoom* Room : Graph->GetAllRooms())
		{
			if (Room->IsInstanceLoaded())
				RoomCount++;
		}

		if (RoomCount == Graph->Count())
			SetState(EGenerationState::Initialization);
		break;
	case EGenerationState::Initialization:
		for (URoom* Room : Graph->GetAllRooms())
		{
			if (Room->IsInstanceInitialized())
				RoomCount++;
		}

		if (RoomCount == Graph->Count())
			SetState(EGenerationState::Play);
		break;
	case EGenerationState::Play:
		UpdateRoomVisibility();
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
	case EGenerationState::Unload:
		Graph->Clear();
		GetWorld()->FlushLevelStreaming();
		GEngine->ForceGarbageCollection(true);
		LogInfo("======= End Unload All Levels =======");
		break;
	case EGenerationState::Generation:
		LogInfo("======= End Map Generation =======");
		break;
	case EGenerationState::Load:
		LogInfo("======= End Load All Levels =======");
		break;
	case EGenerationState::Initialization:
		LogInfo("======= End Init All Levels =======");

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
		{
			LogInfo("Rebuild navmesh");
			nav->CancelBuild();
			nav->Build();
		}

		// Invoke Post Generation Event when initialization is done
		DispatchPostGeneration();
		break;
	case EGenerationState::Play:
		LogInfo("======= End Play =======");
		break;
	default:
		break;
	}
}

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

void ADungeonGenerator::DispatchPreGeneration()
{
	OnPreGeneration();
	OnPreGeneration_BP();
	OnPreGenerationEvent.Broadcast();
}

void ADungeonGenerator::DispatchPostGeneration()
{
	OnPostGeneration();
	OnPostGeneration_BP();
	OnPostGenerationEvent.Broadcast();
}

void ADungeonGenerator::DispatchGenerationInit()
{
	OnGenerationInit();
	OnGenerationInit_BP();
	OnGenerationInitEvent.Broadcast();
}

void ADungeonGenerator::DispatchGenerationFailed()
{
	OnGenerationFailed();
	OnGenerationFailed_BP();
	OnGenerationFailedEvent.Broadcast();
}

void ADungeonGenerator::DispatchRoomAdded(const URoomData* NewRoom)
{
	OnRoomAdded(NewRoom);
	OnRoomAdded_BP(NewRoom);
	OnRoomAddedEvent.Broadcast(NewRoom);
}

URoomData* ADungeonGenerator::GetRandomRoomData(TArray<URoomData*> RoomDataArray)
{
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
