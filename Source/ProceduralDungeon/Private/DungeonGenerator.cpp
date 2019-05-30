// Fill out your copyright notice in the Description page of Project Settings.

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


// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RandomSeed = true;
	AutoIncrementSeed = false;
	Seed = 123456789; // default Seed

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
	OnStateTick(State);
}

void ADungeonGenerator::Generate()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
	{
		if (RandomSeed)
		{
			Random.GenerateNewSeed();
			Seed = Random.GetCurrentSeed();
		}

		BeginGeneration(Seed);

		if (!RandomSeed && AutoIncrementSeed)
		{
			Seed += 123456;
		}
	}
}

void ADungeonGenerator::BeginGeneration_Implementation(uint32 _Seed)
{
	Seed = _Seed;
	SetState(EGenerationState::Unload);
}

void ADungeonGenerator::CreateDungeon()
{
	IsInit = false;
	NbInitRoom = 0;
	int TriesLeft = MaxTry;

	// generate level until there IsValidDungeon return true
	do {
		// Reset generation data
		UProceduralLevelStreaming::UniqueLevelInstanceId = 0;
		ARoomLevel::Count = 0;
		OnGenerationInit_BP();

		// Create the first room
		RoomList.Empty();
		URoom* root = NewObject<URoom>();
		root->Init(ChooseFirstRoomData());
		RoomList.Add(root);

		// Build the list of rooms with recursive function
		AddRoom(*root);

		TriesLeft--;
	} while (TriesLeft > 0 && IsValidDungeon_BP());
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
		EDirection DoorRot = Room->GetDoorWorldOrientation(i);

		// Don't instantiate room nor door if it's the parent
		if (r != Room->Parent)
		{
			TSubclassOf<ADoor> DoorClass = ChooseDoor(Room->GetRoomDataClass(), nullptr != r ? r->GetRoomDataClass() : nullptr);

			if (DoorClass != nullptr)
			{
				FVector InstanceDoorPos = URoom::GetRealDoorPosition(DoorCell, DoorRot); // URoom::Unit() * (FVector(doorCell) + 0.5f * FVector(URoom::GetDirection(DoorRot)) + FVector(0, 0, URoom::DoorOffset()));
				FRotator InstanceDoorRot = FRotator(0, -90 * (int8)DoorRot, 0);
				ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot);

				if (nullptr != Door)
				{
					Door->SetConnectingRooms(Room, r);
					DoorList.Add(Door);
				}
				else
				{
					UE_LOG(LogProceduralDungeon, Error, TEXT("Failed to spawn Door, make sure you set door actor to always spawning."));
				}
			}
		}
	}
}

void ADungeonGenerator::AddRoom(URoom& Room)
{
	int nbDoor = Room.Values->GetNbDoor();

	for (int i = 0; i < nbDoor; i++)
	{
		if (!Room.IsConnected(i))
		{
			TSubclassOf<URoomData> def = ChooseNextRoomData(Room.GetRoomDataClass());
			URoomData* defaultObject = def.GetDefaultObject();

			// Create room from roomdef and set connections with current room
			URoom* newRoom = NewObject<URoom>();
			newRoom->Init(def, &Room);
			int doorIndex = defaultObject->RandomDoor ? 0 : Random.RandRange(0, newRoom->Values->GetNbDoor() - 1);
			
			// Set position, rotation and connections between new room and parent room
			newRoom->ConnectTo(doorIndex, Room, i);

			if (!URoom::Overlap(*newRoom, RoomList))
			{
				RoomList.Add(newRoom);

				OnRoomAdded(newRoom->GetRoomDataClass());

				if(ContinueGeneration_BP())
				{
					AddRoom(*newRoom);
				}
			}
			else
			{
				delete newRoom;
				Room.SetConnection(i, nullptr);
			}
		}
	}
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
	OnStateEnd(State);
	PreviousState = State;
	State = NewState;
	OnStateBegin(State);
}

void ADungeonGenerator::OnStateBegin(EGenerationState _State)
{
	switch (State)
	{
	case EGenerationState::Unload:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= Begin Unload All Levels ======="));
		UnloadAllRooms();
		NbUnloadedRoom = 0;
		break;
	case EGenerationState::Generation:
		//GetWorld()->GetGameState<AGameState>()->OnPreGeneration.Broadcast();
		UE_LOG(LogProceduralDungeon, Log, TEXT("Seed: %d"), Seed);
		OnPreGenerationEvent.Broadcast();
		OnPreGeneration_BP();
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= Begin Map Generation ======="));
		CreateDungeon();
		break;
	case EGenerationState::Load:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= Begin Load All Levels ======="));
		LoadAllRooms();
		NbLoadedRoom = 0;
		break;
	case EGenerationState::Initialization:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= Begin Init All Levels ======="));
		UE_LOG(LogProceduralDungeon, Log, TEXT("Nb Room To Initialize: %d"), RoomList.Num());
		break;
	default:
		break;
	}
}

void ADungeonGenerator::OnStateTick(EGenerationState _State)
{
	int tmp = 0;
	switch (State)
	{
	case EGenerationState::Unload:

		// Count nb level loaded
		for (int i = 0; i < RoomList.Num(); i++)
		{
			if (RoomList[i]->Instance->IsLevelUnloaded())
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
			if (RoomList[i]->Instance->IsLevelLoaded())
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

						UE_LOG(LogProceduralDungeon, Log, TEXT("Room Initialization: %d/%d"), NbInitRoom, RoomList.Num());
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

void ADungeonGenerator::OnStateEnd(EGenerationState _State)
{
	FTimerHandle handle;
	UNavigationSystemV1* nav = nullptr;
	switch (State)
	{
	case EGenerationState::Unload:
		RoomList.Empty();
		GetWorld()->FlushLevelStreaming();
		GEngine->ForceGarbageCollection(true);
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= End Unload All Levels ======="));
		break;
	case EGenerationState::Generation:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= End Map Generation ======="));
		break;
	case EGenerationState::Load:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= End Load All Levels ======="));
		break;
	case EGenerationState::Initialization:
		UE_LOG(LogProceduralDungeon, Log, TEXT("======= End Init All Levels ======="));

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
		{
			UE_LOG(LogProceduralDungeon, Log, TEXT("Build navmesh"));
			nav->CancelBuild();
			nav->Build();
		}
		else
		{
			UE_LOG(LogProceduralDungeon, Error, TEXT("Error: Navmesh not found"));
		}

		// Invoke Post Generation Event when initialization is done
		OnPostGenerationEvent.Broadcast();
		OnPostGeneration_BP();
		break;
	default:
		break;
	}
}

TSubclassOf<URoomData> ADungeonGenerator::GetRandomRoomData(TArray<TSubclassOf<URoomData>> RoomDataArray)
{
	int n = Random.RandRange(0, RoomDataArray.Num() - 1);
	return RoomDataArray[n];
}

bool ADungeonGenerator::HasAlreadyRoomData(TSubclassOf<URoomData> RoomData)
{
	bool hasIt = false;
	for (int i = 0; i < RoomList.Num(); i++)
	{
		if (RoomList[i]->GetRoomDataClass() == RoomData)
		{
			hasIt = true;
			break;
		}
	}
	return  hasIt;
}
