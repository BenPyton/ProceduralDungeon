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
#include "ProceduralDungeonSettings.h"

template<typename T>
class TQueueOrStack
{
public:
	enum class Mode { QUEUE, STACK };

	TQueueOrStack(Mode _mode) : m_mode(_mode), m_queue(), m_stack() {}

	void Push(T& _element)
	{
		switch(m_mode)
		{
		case Mode::QUEUE:
			m_queue.Enqueue(_element);
			break;
		case Mode::STACK:
			m_stack.Push(_element);
			break;
		}
	}

	T Pop()
	{
		check(!IsEmpty());
		T item = T();
		switch(m_mode)
		{
		case Mode::QUEUE:
			m_queue.Dequeue(item);
			break;
		case Mode::STACK:
			item = m_stack.Pop();
			break;
		}
		return item;
	}

	int Num()
	{
		switch(m_mode)
		{
		case Mode::QUEUE:
			return m_queue.Num();
		case Mode::STACK:
			return m_stack.Num();
		}
	}

	bool IsEmpty()
	{
		switch(m_mode)
		{
		case Mode::QUEUE:
			return m_queue.IsEmpty();
		case Mode::STACK:
			return m_stack.Num() <= 0;
		}
		return true;
	}

private:
	Mode m_mode;
	TQueue<T> m_queue;
	TArray<T> m_stack;
};

bool ShowLogsOnScreen(float& _duration)
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	_duration = Settings->PrintDebugDuration;
	return Settings->OnScreenPrintDebug;
}

void LogInfo(FString message, bool showOnScreen = true)
{
	UE_LOG(LogProceduralDungeon, Log, TEXT("%s"), *message);
	float duration;
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::White, message);
	}
}

void LogWarning(FString message, bool showOnScreen = true)
{
	UE_LOG(LogProceduralDungeon, Warning, TEXT("%s"), *message);
	float duration;
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Yellow, message);
	}
}

void LogError(FString message, bool showOnScreen = true)
{
	UE_LOG(LogProceduralDungeon, Error, TEXT("%s"), *message);
	float duration;
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Red, message);
	}
}

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GenerationType = EGenerationType::DFS;
	SeedType = ESeedType::Random;
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

void ADungeonGenerator::BeginGeneration_Implementation(uint32 _Seed)
{
	Seed = _Seed;
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
		// Reset generation data
		UProceduralLevelStreaming::UniqueLevelInstanceId = 0;
		ARoomLevel::Count = 0;
		DispatchGenerationInit();

		// Create the first room
		RoomList.Empty();
		URoom* root = NewObject<URoom>();
		root->Init(ChooseFirstRoomData());
		RoomList.Add(root);

		// Create the list with the correct mode (depth or breadth)
		TQueueOrStack<URoom*>::Mode listMode;
		switch(GenerationType)
		{
		case EGenerationType::DFS:
			listMode = TQueueOrStack<URoom*>::Mode::STACK;
			break;
		case EGenerationType::BFS:
			listMode = TQueueOrStack<URoom*>::Mode::QUEUE;
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

		TriesLeft--;
	} while (TriesLeft > 0 && !IsValidDungeon());
}


void ADungeonGenerator::InstantiateRoom(URoom* _Room)
{
	// Instantiate room
	_Room->Instantiate(GetWorld());

	for (int i = 0; i < _Room->GetConnectionCount(); i++)
	{
		// Get next room
		URoom* r = _Room->GetConnection(i).Get();

		FIntVector DoorCell = _Room->GetDoorWorldPosition(i);
		EDoorDirection DoorRot = _Room->GetDoorWorldOrientation(i);

		// Don't instantiate room nor door if it's the parent
		if (r != _Room->Parent)
		{
			TSubclassOf<ADoor> DoorClass = ChooseDoor(_Room->GetRoomDataClass(), nullptr != r ? r->GetRoomDataClass() : nullptr);

			if (DoorClass != nullptr)
			{
				FVector InstanceDoorPos = URoom::GetRealDoorPosition(DoorCell, DoorRot); // URoom::Unit() * (FVector(doorCell) + 0.5f * FVector(URoom::GetDirection(DoorRot)) + FVector(0, 0, URoom::DoorOffset()));
				FRotator InstanceDoorRot = FRotator(0, -90 * (int8)DoorRot, 0);
				ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot);

				if (nullptr != Door)
				{
					Door->SetConnectingRooms(_Room, r);
					DoorList.Add(Door);
				}
				else
				{
					LogError("Failed to spawn Door, make sure you set door actor to always spawning.");
				}
			}
		}
	}
}

TArray<URoom*> ADungeonGenerator::AddNewRooms(URoom& _ParentRoom)
{
	TArray<URoom*> newRooms;
	int nbDoor = _ParentRoom.Values->GetNbDoor();
	URoom* newRoom = nullptr;
	for(int i = 0; i < nbDoor; ++i)
	{
		int nbTries = MaxRoomTry;
		do
		{
			nbTries--;
			TSubclassOf<URoomData> def = ChooseNextRoomData(_ParentRoom.GetRoomDataClass());
			URoomData* defaultObject = def.GetDefaultObject();

			// Create room from roomdef and set connections with current room
			newRoom = NewObject<URoom>();
			newRoom->Init(def, &_ParentRoom);
			int doorIndex = defaultObject->RandomDoor ? Random.RandRange(0, newRoom->Values->GetNbDoor() - 1) : 0;

			// Set position, rotation and connections between new room and parent room
			newRoom->ConnectTo(doorIndex, _ParentRoom, i);

			if(!URoom::Overlap(*newRoom, RoomList))
			{
				RoomList.Add(newRoom);
				DispatchRoomAdded(newRoom->GetRoomDataClass());
			}
			else
			{
				newRoom = nullptr;
				_ParentRoom.SetConnection(i, nullptr);
			}
		} while(nbTries > 0 && newRoom == nullptr);

		if(newRoom != nullptr)
		{
			newRooms.Add(newRoom);
		}
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

TSubclassOf<URoomData> ADungeonGenerator::ChooseFirstRoomData_Implementation()
{
	LogError("Error: ChooseFirstRoomData not implemented");
	return nullptr;
}

TSubclassOf<URoomData> ADungeonGenerator::ChooseNextRoomData_Implementation(TSubclassOf<URoomData> CurrentRoom)
{
	LogError("Error: ChooseNextRoomData not implemented");
	return nullptr;
}

TSubclassOf<ADoor> ADungeonGenerator::ChooseDoor_Implementation(TSubclassOf<URoomData> CurrentRoom, TSubclassOf<URoomData> NextRoom)
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

void ADungeonGenerator::DispatchRoomAdded(TSubclassOf<URoomData> NewRoom)
{
	OnRoomAdded(NewRoom);
	OnRoomAdded_BP(NewRoom);
	OnRoomAddedEvent.Broadcast(NewRoom);
}

TSubclassOf<URoomData> ADungeonGenerator::GetRandomRoomData(TArray<TSubclassOf<URoomData>> _RoomDataArray)
{
	int n = Random.RandRange(0, _RoomDataArray.Num() - 1);
	return _RoomDataArray[n];
}

bool ADungeonGenerator::HasAlreadyRoomData(TSubclassOf<URoomData> _RoomData)
{
	return CountRoomData(_RoomData) > 0;
}

bool ADungeonGenerator::HasAlreadyOneRoomDataFrom(TArray<TSubclassOf<URoomData>> _RoomDataList)
{
	return CountTotalRoomData(_RoomDataList) > 0;
}

int ADungeonGenerator::CountRoomData(TSubclassOf<URoomData> _RoomData)
{
	int count = 0;
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(RoomList[i]->GetRoomDataClass() == _RoomData)
		{
			count++;
		}
	}
	return  count;
}

int ADungeonGenerator::CountTotalRoomData(TArray<TSubclassOf<URoomData>> _RoomDataList)
{
	int count = 0;
	for(int i = 0; i < RoomList.Num(); i++)
	{
		if(_RoomDataList.Contains(RoomList[i]->GetRoomDataClass()))
		{
			count++;
		}
	}
	return  count;
}
