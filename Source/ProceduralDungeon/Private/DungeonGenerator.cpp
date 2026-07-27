// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "DungeonGenerator.h"
#include "RoomData.h"
#include "Room.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGraph.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	GenerationType = EGenerationType::DFS;
}

bool ADungeonGenerator::CreateDungeon_Implementation()
{
	// Only server generate the dungeon
	// DungeonGraph will be replicated to all clients
	if (!HasAuthority())
		return false;

	TRACE_CPUPROFILER_EVENT_SCOPE(ADungeonGenerator::CreateDungeon);
	switch (CurrentState)
	{
	case EState::Idle:
		DungeonLog_Debug("--- Idle State");
		// Maybe move from plugin settings to generator's variable?
		CurrentTriesLeft = Dungeon::MaxGenerationTryBeforeGivingUp();
		CurrentState = EState::Initializing;
		// No break to execute immediatly the Initializing state
	case EState::Initializing: {
		DungeonLog_Debug("--- Initializing State");
		--CurrentTriesLeft;

		// Reset generation data
		StartNewDungeon();

		// Create the list with the correct mode (depth or breadth)
		TQueueOrStack<URoom*>::EMode ListMode;
		switch (GenerationType)
		{
		case EGenerationType::DFS:
			ListMode = TQueueOrStack<URoom*>::EMode::STACK;
			break;
		case EGenerationType::BFS:
			ListMode = TQueueOrStack<URoom*>::EMode::QUEUE;
			break;
		default:
			DungeonLog_Error("GenerationType value is not supported.");
			return false;
		}

		URoomData* FirstRoomData = nullptr;
		FRoomTransform FirstRoomTransform;
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(ADungeonGenerator::ChooseFirstRoomData);
			FirstRoomData = ChooseFirstRoomData(FirstRoomTransform);
		}
		if (!IsValid(FirstRoomData))
		{
			DungeonLog_Error("ChooseFirstRoomData returned null.");
		}
		else
		{
			// Create the first room
			URoom* FirstRoom = CreateRoomInstance(FirstRoomData);
			FirstRoom->SetRoomTransform(FirstRoomTransform);
			AddRoomToDungeon(FirstRoom, /*DoorsToConnect = */ {}, /*bFailIfNotConnected = */ false);

			// Build the list of rooms
			PendingRooms.SetMode(ListMode);
			PendingRooms.Push(FirstRoom);

			CurrentState = EState::AddingRooms;
		}
		// No break to execute immediatly the AddingRooms state
	}
	case EState::AddingRooms: {
		DungeonLog_Debug("--- AddingRooms State");

		TArray<URoom*> NewRooms;
		int BatchCount = RoomBatchSize;
		while (!PendingRooms.IsEmpty() && BatchCount > 0)
		{
			--BatchCount;
			URoom* CurrentRoom = PendingRooms.Pop();
			check(IsValid(CurrentRoom)); // CurrentRoom should always be valid

			if (!AddNewRooms(*CurrentRoom, NewRooms))
			{
				// Stop generation here
				DungeonLog_Debug("--- Stopping generation as AddNewRooms returned false.");
				PendingRooms.Empty();
				break;
			}

			DungeonLog_Debug("--- %d rooms added to the dungeon.", NewRooms.Num());
			for (URoom* Room : NewRooms)
			{
				PendingRooms.Push(Room);
			}
		}

		if (!PendingRooms.IsEmpty())
		{
			DungeonLog_Debug("--- Still pending rooms, yielding.");
		}
		else
		{
			DungeonLog_Debug("--- No more pending rooms, finalizing.");
			CurrentState = EState::Finalizing;
		}
		// Proceed to next tick
		YieldGeneration();
		break;
	}
	case EState::Finalizing:
		DungeonLog_Debug("--- Finalizing State");
		// Initialize the dungeon by eg. altering the room instances
		FinalizeDungeon();
		CurrentState = EState::Idle;
		if (!IsValidDungeon())
		{
			DungeonLog_Debug("--- Dungeon is not valid, tries left: %d", CurrentTriesLeft);
			if (CurrentTriesLeft <= 0)
			{
				DungeonLog_Error("Generated dungeon is not valid after %d tries. Make sure your ChooseNextRoomData and IsValidDungeon functions are correct.", Dungeon::MaxGenerationTryBeforeGivingUp());
				return false;
			}
			else
			{
				CurrentState = EState::Initializing;
				YieldGeneration();
			}
		}
		break;
	default:
		DungeonLog_Error("CurrentState value is not supported.");
		return false;
	}

	return true;
}

bool ADungeonGenerator::AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms)
{
	check(HasAuthority());

	TRACE_CPUPROFILER_EVENT_SCOPE(ADungeonGenerator::AddNewRooms);
	int NbDoor = ParentRoom.GetRoomData()->GetNbDoor();
	if (NbDoor <= 0)
	{
		DungeonLog_Error("The room data '%s' has no door! Nothing could be generated with it!", *GetNameSafe(ParentRoom.GetRoomData()));
	}

	// Cache world before loops
	const UWorld* World = GetWorld();
	const FBoxMinAndMax DungeonBounds = DungeonLimits.GetBox();

	AddedRooms.Reset();
	bool ShouldContinue = false;
	for (int i = 0; ShouldContinue = ContinueToAddRoom(), i < NbDoor && ShouldContinue; ++i)
	{
		if (ParentRoom.IsConnected(i))
			continue;

		TRACE_CPUPROFILER_EVENT_SCOPE(ADungeonGenerator::AddNewRooms::Loop);
		// Get the door definition in its world position and direction
		FDoorDef DoorDef = ParentRoom.GetDoorDef(i);

		// Get the door definition for the next room
		const FDoorDef NewRoomDoor = DoorDef.GetOpposite();
		if (!DungeonBounds.IsInside(NewRoomDoor.Transform.Translation))
			continue;

		// Maybe move from plugin settings to generator's variable?
		int NbTries = Dungeon::MaxRoomPlacementTryBeforeGivingUp();
		URoom* NewRoom = nullptr;
		int DoorIndex = -1;
		// Try to place a new room
		do
		{
			NbTries--;
			bDiscardRoom = false;
			URoomData* RoomDef = nullptr;
			{
				TRACE_CPUPROFILER_EVENT_SCOPE(ADungeonGenerator::ChooseNextRoomData);
				RoomDef = ChooseNextRoomData(ParentRoom.GetRoomData(), &ParentRoom, DoorDef, DoorIndex);
			}
			if (!IsValid(RoomDef))
			{
				bDiscardRoom |= bAutoDiscardRoomIfNull;
				if (bDiscardRoom)
				{
					break;
				}
				else
				{
					DungeonLog_Error("ChooseNextRoomData returned null.");
					continue;
				}
			}

			if (DoorIndex >= RoomDef->Doors.Num())
			{
				DungeonLog_Error("ChooseNextRoomData returned door index '%d' which is out of range in the RoomData '%s' door list (max: %d).", DoorIndex, *RoomDef->GetName(), RoomDef->Doors.Num() - 1);
				continue;
			}

			// Get all compatible door indices from the chosen room data
			TArray<int> compatibleDoors;
			RoomDef->GetCompatibleDoors(DoorDef, compatibleDoors);
			if (compatibleDoors.Num() <= 0)
			{
				DungeonLog_Error("ChooseNextRoomData returned room data '%s' with no compatible door (door type: '%s').", *RoomDef->GetName(), *DoorDef.GetTypeName());
				continue;
			}

			// Get only doors if the new room could fit in the dungeon bounds
			for (int n = compatibleDoors.Num() - 1; n >= 0; --n)
			{
				if (!RoomDef->IsRoomInBounds(DungeonBounds, compatibleDoors[n], NewRoomDoor))
					compatibleDoors.RemoveAt(n);
			}

			if (compatibleDoors.Num() <= 0)
			{
				DungeonLog_Warning("ChooseNextRoomData returned room data '%s' that could not fit in dungeon bounds.", *RoomDef->GetName());
				continue;
			}

			if (RoomDef->RandomDoor || (DoorIndex < 0))
				DoorIndex = compatibleDoors[GetRandomStream().RandRange(0, compatibleDoors.Num() - 1)];
			else if (!compatibleDoors.Contains(DoorIndex))
			{
				DungeonLog_Error("ChooseNextRoomData returned door index '%d' (RoomData '%s') which its type '%s' is not compatible with '%s'.", DoorIndex, *RoomDef->GetName(), *RoomDef->Doors[DoorIndex].GetTypeName(), *DoorDef.GetTypeName());
				continue;
			}

			// Create new room instance from roomdef
			NewRoom = CreateRoomInstance(RoomDef);

			// Place the room at targeted door position if possible
			if (!TryPlaceRoom(NewRoom, DoorIndex, NewRoomDoor, World))
			{
				DiscardRoomInstance(NewRoom);
			}
		} while (NbTries > 0 && NewRoom == nullptr);

		// If we explicitely want to not place a room, then goes to next door
		if (bDiscardRoom)
			continue;

		// Plugin-wide setting is deprecated, will be removed in v4.0
		const bool bConnectAllDoors = bCanLoop && Dungeon::CanLoop();
		if (AddRoomToDungeon(NewRoom, bConnectAllDoors ? TArray<int> {} : TArray<int> {DoorIndex}))
		{
			AddedRooms.Add(NewRoom);
		}
		else // No room can be placed and all placement tries exhausted
		{
			// @TODO: Find a way to move this call in AddRoomToDungeon
			OnFailedToAddRoom(ParentRoom.GetRoomData(), DoorDef);
		}
	}

	// Maybe move from plugin settings to generator's variable?
	const bool bRoomLimitReached = Graph->Count() > Dungeon::RoomLimit();
	if (bRoomLimitReached)
	{
		DungeonLog_Warning("Dungeon has reached the room limit of %d! Check your 'Continue To Add Room' to make sure your dungeon is not in an infinite loop, or increase the room limit in the plugin settings if this is intentional.", Dungeon::RoomLimit());
	}

	return ShouldContinue && !bRoomLimitReached;
}

// ===== Default Native Events Implementations =====

URoomData* ADungeonGenerator::ChooseFirstRoomData_Implementation(FRoomTransform& Transform)
{
	DungeonLog_Error("Error: ChooseFirstRoomData not implemented");
	return nullptr;
}

URoomData* ADungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)
{
	DungeonLog_Error("Error: ChooseNextRoomData not implemented");
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

// ===== Utility Functions (Deprectated!!!) =====

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
