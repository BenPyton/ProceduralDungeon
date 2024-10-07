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
#include "RoomData.h"
#include "Room.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "QueueOrStack.h"
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

	// Maybe move from plugin settings to generator's variable?
	int TriesLeft = Dungeon::MaxGenerationTryBeforeGivingUp();
	bool ValidDungeon = false;

	// generate level until IsValidDungeon return true
	do {
		TriesLeft--;

		// Reset generation data
		StartNewDungeon();

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
			return false;
		}

		URoomData* def = ChooseFirstRoomData();
		if (!IsValid(def))
		{
			DungeonLog_Error("ChooseFirstRoomData returned null.");
			continue;
		}

		// Create the first room
		URoom* root = CreateRoomInstance(def);
		AddRoomToDungeon(root, /*DoorsToConnect = */{}, /*bFailIfNotConnected = */false);

		// Build the list of rooms
		TQueueOrStack<URoom*> roomStack(listMode);
		roomStack.Push(root);
		URoom* currentRoom = nullptr;
		TArray<URoom*> newRooms;
		while (!roomStack.IsEmpty())
		{
			currentRoom = roomStack.Pop();
			check(IsValid(currentRoom)); // currentRoom should always be valid

			if (!AddNewRooms(*currentRoom, newRooms))
				break;

			for (URoom* room : newRooms)
			{
				roomStack.Push(room);
			}
		}

		// Initialize the dungeon by eg. altering the room instances
		FinalizeDungeon();

		ValidDungeon = IsValidDungeon();
	} while (TriesLeft > 0 && !ValidDungeon);

	if (!ValidDungeon)
	{
		DungeonLog_Error("Generated dungeon is not valid after %d tries. Make sure your ChooseNextRoomData and IsValidDungeon functions are correct.", Dungeon::MaxGenerationTryBeforeGivingUp());
		return false;
	}

	return true;
}

bool ADungeonGenerator::AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms)
{
	check(HasAuthority());

	int nbDoor = ParentRoom.GetRoomData()->GetNbDoor();
	if (nbDoor <= 0)
		DungeonLog_Error("The room data '%s' has no door! Nothing could be generated with it!", *GetNameSafe(ParentRoom.GetRoomData()));

	// Cache world before loops
	const UWorld* World = GetWorld();
	const FBoxMinAndMax DungeonBounds = DungeonLimits.GetBox();

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

		// Get the door definition for the next room
		const FDoorDef newRoomDoor = doorDef.GetOpposite();
		if (!DungeonBounds.IsInside(newRoomDoor.Position))
			continue;

		// Maybe move from plugin settings to generator's variable?
		int nbTries = Dungeon::MaxRoomPlacementTryBeforeGivingUp();
		URoom* newRoom = nullptr;
		int doorIndex = -1;
		// Try to place a new room
		do
		{
			nbTries--;
			bDiscardRoom = false;
			URoomData* roomDef = ChooseNextRoomData(ParentRoom.GetRoomData(), &ParentRoom, doorDef, doorIndex);
			if (!IsValid(roomDef))
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

			// Get only doors if the new room could fit in the dungeon bounds
			for (int n = compatibleDoors.Num() - 1; n >= 0; --n)
			{
				if (!roomDef->IsRoomInBounds(DungeonBounds, compatibleDoors[n], newRoomDoor))
					compatibleDoors.RemoveAt(n);
			}

			if (compatibleDoors.Num() <= 0)
			{
				DungeonLog_Warning("ChooseNextRoomData returned room data '%s' that could not fit in dungeon bounds.", *roomDef->GetName());
				continue;
			}

			if (roomDef->RandomDoor || (doorIndex < 0))
				doorIndex = compatibleDoors[GetRandomStream().RandRange(0, compatibleDoors.Num() - 1)];
			else if (!compatibleDoors.Contains(doorIndex))
			{
				DungeonLog_Error("ChooseNextRoomData returned door index '%d' (RoomData '%s') which its type '%s' is not compatible with '%s'.", doorIndex, *roomDef->GetName(), *roomDef->Doors[doorIndex].GetTypeName(), *doorDef.GetTypeName());
				continue;
			}

			// Create new room instance from roomdef
			newRoom = CreateRoomInstance(roomDef);

			// Place the room at targeted door position if possible
			if (!TryPlaceRoom(newRoom, doorIndex, newRoomDoor, World))
			{
				// The object will be automatically deleted by the GC
				newRoom = nullptr;
			}
		} while (nbTries > 0 && newRoom == nullptr);

		// If we explicitely want to not place a room, then goes to next door
		if (bDiscardRoom)
			continue;

		// Plugin-wide setting is deprecated, will be removed in v4.0
		const bool bConnectAllDoors = bCanLoop && Dungeon::CanLoop();
		if (AddRoomToDungeon(newRoom, bConnectAllDoors ? TArray<int>{} : TArray<int> {doorIndex}))
		{
			AddedRooms.Add(newRoom);
		}
		else // No room can be placed and all placement tries exhausted
		{
			// @TODO: Find a way to move this call in AddRoomToDungeon
			OnFailedToAddRoom(ParentRoom.GetRoomData(), doorDef);
		}
	}

	// Maybe move from plugin settings to generator's variable?
	const bool bRoomLimitReached = Graph->Count() > Dungeon::RoomLimit();
	if (bRoomLimitReached)
	{
		DungeonLog_Warning("Dungeon has reached the room limit of %d! Check your 'Continue To Add Room' to make sure your dungeon is not in an infinite loop, or increase the room limit in the plugin settings if this is intentional.", Dungeon::RoomLimit());
	}

	return shouldContinue && !bRoomLimitReached;
}

// ===== Default Native Events Implementations =====

URoomData* ADungeonGenerator::ChooseFirstRoomData_Implementation()
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
