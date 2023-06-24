/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "DungeonGraph.h"
#include "Room.h"
#include "RoomData.h"

UDungeonGraph::UDungeonGraph()
	: Super()
{
}

void UDungeonGraph::AddRoom(URoom* Room)
{
	Rooms.Add(Room);
}

void UDungeonGraph::GetAllRoomsFromData(const URoomData* Data, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [Data](const URoom* Room) { return Room->GetRoomData() == Data; });
}

void UDungeonGraph::GetAllRoomsFromDataList(const TArray<URoomData*>& Data, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [&Data](const URoom* Room) { return Data.Contains(Room->GetRoomData()); });
}

URoom* UDungeonGraph::GetRandomRoom()
{
	return nullptr;
}

bool UDungeonGraph::HasAlreadyRoomData(const URoomData* RoomData) const
{
	return CountRoomData(RoomData) > 0;
}

bool UDungeonGraph::HasAlreadyOneRoomDataFrom(const TArray<URoomData*>& RoomDataList) const
{
	return CountTotalRoomData(RoomDataList) > 0;
}

int UDungeonGraph::CountRoomData(const URoomData* RoomData) const
{
	return CountRoomByPredicate([RoomData](const URoom* Room) { return Room->GetRoomData() == RoomData; });
}

int UDungeonGraph::CountTotalRoomData(const TArray<URoomData*>& RoomDataList) const
{
	return CountRoomByPredicate([&RoomDataList](const URoom* Room) { return RoomDataList.Contains(Room->GetRoomData()); });
}

bool UDungeonGraph::HasAlreadyRoomType(const TSubclassOf<URoomData>& RoomType) const
{
	return CountRoomType(RoomType) > 0;
}

bool UDungeonGraph::HasAlreadyOneRoomTypeFrom(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const
{
	return CountTotalRoomType(RoomTypeList) > 0;
}

int UDungeonGraph::CountRoomType(const TSubclassOf<URoomData>& RoomType) const
{
	return CountRoomByPredicate([&RoomType](const URoom* Room) { return Room->GetRoomData()->GetClass()->IsChildOf(RoomType); });
}

int UDungeonGraph::CountTotalRoomType(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const
{
	return CountRoomByPredicate([&RoomTypeList](const URoom* Room) {
		return RoomTypeList.ContainsByPredicate([Room](const TSubclassOf<URoomData> RoomType) {
			return Room->GetRoomData()->GetClass()->IsChildOf(RoomType);
		}); 
	});
}

URoom* UDungeonGraph::GetRoomAt(FIntVector RoomCell) const
{
	return URoom::GetRoomAt(RoomCell, Rooms);
}

URoom* UDungeonGraph::GetRoomByIndex(int64 Index) const
{
	for (URoom* Room : Rooms)
	{
		if (Room->GetRoomID() == Index)
			return Room;
	}
	return nullptr;
}

void UDungeonGraph::Clear()
{
	Rooms.Empty();
}

int UDungeonGraph::CountRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const
{
	int count = 0;
	for (const URoom* Room : Rooms)
	{
		if (Predicate(Room))
			count++;
	}
	return  count;
}

void UDungeonGraph::GetRoomsByPredicate(TArray<URoom*>& OutRooms, TFunction<bool(const URoom*)> Predicate) const
{
	OutRooms.Empty();
	for (URoom* Room : Rooms)
	{
		check(IsValid(Room));
		if (Predicate(Room))
		{
			OutRooms.Add(Room);
		}
	}
}

void UDungeonGraph::TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*)> Func)
{
	TSet<URoom*> openList(InRooms);
	TSet<URoom*> closedList, currentList;
	while (Distance > 0 && openList.Num() > 0)
	{
		for (URoom* openRoom : openList)
			closedList.Add(openRoom);

		Swap(currentList, openList);
		openList.Empty();
		for (URoom* currentRoom : currentList)
		{
			Func(currentRoom);
			for (int i = 0; i < currentRoom->GetConnectionCount(); ++i)
			{
				URoom* nextRoom = currentRoom->GetConnection(i).Get();
				if(IsValid(nextRoom) && !closedList.Contains(nextRoom))
					openList.Add(nextRoom);
			}
		}
		--Distance;
	}

	if (OutRooms != nullptr)
		Swap(*OutRooms, closedList);
}
