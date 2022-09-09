/*
 * MIT License
 *
 * Copyright (c) 2019-2022 Benoit Pelletier
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
#include "Engine.h"
#include "NavigationSystem.h"
#include "ProceduralLevelStreaming.h"
#include "RoomData.h"
#include "Room.h"
#include "Door.h"
#include "RoomLevel.h"
#include "ProceduralDungeon.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"
#include "QueueOrStack.h"

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
 }

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
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
	IsInit = false;
	NbInitRoom = 0;
	int TriesLeft = MaxTry;

	// generate level until there IsValidDungeon return true
	do {
		TriesLeft--;

		// Reset generation data
		DispatchGenerationInit();

		// Create the first room
		RoomList.Empty();

		URoomData* def = ChooseFirstRoomData();
		if(!IsValid(def))
		{
			LogError("ChooseFirstRoomData returned null.");
			continue;
		}

		URoom* root = NewObject<URoom>();
		root->Init(def, this, 0);
		RoomList.Add(root);

		// Create the list with the correct mode (depth or breadth)
		TQueueOrStack<URoom*>::EMode listMode;
		switch(GenerationType)
		{
		case EGenerationType::DFS:
			listMode = TQueueOrStack<URoom*>::EMode::STACK;
			break;
		case EGenerationType::BFS:
			listMode = TQueueOrStack<URoom*>::EMode::QUEUE;
			break;
		}

		// Build the list of rooms
		TQueueOrStack<URoom*> roomStack(listMode);
		roomStack.Push(root);
		URoom* currentRoom = nullptr;
		URoom* newRoom = nullptr;
		while(ContinueToAddRoom() && !roomStack.IsEmpty())
		{
			currentRoom = roomStack.Pop();
			check(IsValid(currentRoom)); // currentRoom should always be valid
			for(URoom* room : AddNewRooms(*currentRoom))
			{
				roomStack.Push(room);
			}
		}

	} while (TriesLeft > 0 && !IsValidDungeon());
}

void ADungeonGenerator::InstantiateRoom(URoom* Room)
{
	// Instantiate room
	Room->Instantiate(GetWorld());

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
			TSubclassOf<ADoor> DoorClass = ChooseDoor(Room->GetRoomData(), nullptr != r ? r->GetRoomData() : nullptr);

			if (DoorClass != nullptr)
			{
				FVector InstanceDoorPos = GetDungeonRotation().RotateVector(URoom::GetRealDoorPosition(DoorCell, DoorRot)) + GetDungeonOffset();
				FQuat InstanceDoorRot = GetDungeonRotation() * FRotator(0, -90 * (int8)DoorRot, 0).Quaternion();
				ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot.Rotator());

				if (nullptr != Door)
				{
					DoorList.Add(Door);
					Door->SetConnectingRooms(Room, r);
					Room->SetDoorInstance(i, Door);
					if(IsValid(r))
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

TArray<URoom*> ADungeonGenerator::AddNewRooms(URoom& ParentRoom)
{
	TArray<URoom*> newRooms;
	int nbDoor = ParentRoom.GetRoomData()->GetNbDoor();
	URoom* newRoom = nullptr;
	for(int i = 0; i < nbDoor; ++i)
	{
		if(ParentRoom.IsConnected(i))
			continue;

		int nbTries = MaxRoomTry;
		// Try to place a new room
		do
		{
			nbTries--;
			URoomData* def = ChooseNextRoomData(ParentRoom.GetRoomData());
			if(!IsValid(def))
			{
				LogError("ChooseNextRoomData returned null.");
				continue;
			}

			// Create room from roomdef and set connections with current room
			newRoom = NewObject<URoom>();
			newRoom->Init(def, this, RoomList.Num());
			int doorIndex = def->RandomDoor ? Random.RandRange(0, newRoom->GetRoomData()->GetNbDoor() - 1) : 0;

			// Place the room at its world position with the correct rotation
			EDoorDirection parentDoorDir = ParentRoom.GetDoorWorldOrientation(i);
			FIntVector newRoomPos = ParentRoom.GetDoorWorldPosition(i) + URoom::GetDirection(parentDoorDir);
			newRoom->SetPositionAndRotationFromDoor(doorIndex, newRoomPos, URoom::Opposite(parentDoorDir));

			// Test if it fit in the place
			if(!URoom::Overlap(*newRoom, RoomList))
			{
				// connect the doors to all possible existing rooms
				URoom::Connect(*newRoom, doorIndex, ParentRoom, i);
				if(URoom::CanLoop())
				{
					newRoom->TryConnectToExistingDoors(RoomList);
				}
				RoomList.Add(newRoom);
				newRooms.Add(newRoom);
				DispatchRoomAdded(newRoom->GetRoomData());
			}
			else
			{
				newRoom = nullptr;
			}
		} while(nbTries > 0 && newRoom == nullptr);
	}

	return newRooms;
}

void ADungeonGenerator::LoadAllRooms()
{
	// When a level is correct, load all rooms
	for (int i = 0; i < RoomList.Num(); i++)
	{
		InstantiateRoom(RoomList[i]);
	}
}

void ADungeonGenerator::UnloadAllRooms()
{
	for (int i = 0; i < DoorList.Num(); i++)
	{
		DoorList[i]->Destroy();
	}
	DoorList.Empty();

	for (int i = 0; i < RoomList.Num(); i++)
	{
		RoomList[i]->Destroy(GetWorld());
	}
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
		UnloadAllRooms();
		NbUnloadedRoom = 0;
		break;
	case EGenerationState::Generation:
		DispatchPreGeneration();
		LogInfo("======= Begin Map Generation =======");
		CreateDungeon();
		break;
	case EGenerationState::Load:
		LogInfo("======= Begin Load All Levels =======");
		LoadAllRooms();
		NbLoadedRoom = 0;
		break;
	case EGenerationState::Initialization:
		LogInfo("======= Begin Init All Levels =======");
		LogInfo(FString::Printf(TEXT("Nb Room To Initialize: %d"), RoomList.Num()));
		break;
	default:
		break;
	}
}

void ADungeonGenerator::OnStateTick(EGenerationState State)
{
	int tmp = 0;
	switch (State)
	{
	case EGenerationState::Unload:

		// Count nb level loaded
		for (int i = 0; i < RoomList.Num(); i++)
		{
			if (RoomList[i]->IsInstanceUnloaded())
			{
				tmp++;
			}
		}
		// Change state when all levels are loaded
		if (tmp == RoomList.Num())
		{
			SetState(EGenerationState::Generation);
		}
		break;
	case EGenerationState::Generation:
		SetState(EGenerationState::Load);
		break;
	case EGenerationState::Load:

		// Count nb level loaded
		for (int i = 0; i < RoomList.Num(); i++)
		{
			if (RoomList[i]->IsInstanceLoaded())
			{
				tmp++;
			}
		}
		// Change state when all levels are loaded
		if (tmp == RoomList.Num())
		{
			SetState(EGenerationState::Initialization);
		}

		break;
	case EGenerationState::Initialization:
		// While initialization isn't done, try to initialize all rooms
		if (!IsInit)
		{
			IsInit = true;
			for (URoom* room : RoomList)
			{
				ARoomLevel* script = room->GetLevelScript();

				if (nullptr != script)
				{
					IsInit &= script->IsInit;
					if (!script->IsInit && !script->PendingInit)
					{
						NbInitRoom++;
						script->Room = nullptr;
						script->Init(room);

						LogInfo(FString::Printf(TEXT("Room Initialization: %d/%d"), NbInitRoom, RoomList.Num()), false);
					}
				}
				else
				{
					IsInit = false;
				}
			}

			if (IsInit)
			{
				SetState(EGenerationState::None);
			}
			return;
		}
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
		RoomList.Empty();
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
	default:
		break;
	}
}

URoomData* ADungeonGenerator::ChooseFirstRoomData_Implementation()
{
	LogError("Error: ChooseFirstRoomData not implemented");
	return nullptr;
}

URoomData* ADungeonGenerator::ChooseNextRoomData_Implementation(URoomData* CurrentRoom)
{
	LogError("Error: ChooseNextRoomData not implemented");
	return nullptr;
}

TSubclassOf<ADoor> ADungeonGenerator::ChooseDoor_Implementation(URoomData* CurrentRoom, URoomData* NextRoom)
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

void ADungeonGenerator::DispatchRoomAdded(URoomData* NewRoom)
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

URoom* ADungeonGenerator::GetRoomAt(FIntVector RoomCell)
{
	return URoom::GetRoomAt(RoomCell, RoomList);
}

bool ADungeonGenerator::HasAlreadyRoomData(URoomData* RoomData)
{
	return CountRoomData(RoomData) > 0;
}

bool ADungeonGenerator::HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList)
{
	return CountTotalRoomData(RoomDataList) > 0;
}

int ADungeonGenerator::CountRoomData(URoomData* RoomData)
{
	int count = 0;
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(RoomList[i]->GetRoomData() == RoomData)
		{
			count++;
		}
	}
	return  count;
}

int ADungeonGenerator::CountTotalRoomData(TArray<URoomData*> RoomDataList)
{
	int count = 0;
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(RoomDataList.Contains(RoomList[i]->GetRoomData()))
		{
			count++;
		}
	}
	return  count;
}

bool ADungeonGenerator::HasAlreadyRoomType(TSubclassOf<URoomData> RoomType)
{
	return CountRoomType(RoomType) > 0;
}

bool ADungeonGenerator::HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList)
{
	return CountTotalRoomType(RoomTypeList) > 0;
}

int ADungeonGenerator::CountRoomType(TSubclassOf<URoomData> RoomType)
{
	int count = 0;
	for (int i = 0; i < RoomList.Num(); i++)
	{
		if (RoomList[i]->GetRoomData()->GetClass()->IsChildOf(RoomType))
		{
			count++;
		}
	}
	return  count;
}

int ADungeonGenerator::CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList)
{
	int count = 0;
	for (int i = 0; i < RoomList.Num(); i++)
	{
		URoomData* roomData = RoomList[i]->GetRoomData();
		if (RoomTypeList.ContainsByPredicate([&roomData](const TSubclassOf<URoomData> roomType) { return roomData->GetClass()->IsChildOf(roomType); } ))
		{
			count++;
		}
	}
	return  count;
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