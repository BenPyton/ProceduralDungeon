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

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonGraph.generated.h"

class URoom;
class URoomData;

UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDungeonGraph : public UObject
{
	GENERATED_BODY()

	friend class ADungeonGenerator;

public:
	UDungeonGraph();
	void AddRoom(URoom* Room);

	// Returns all rooms
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	const TArray<URoom*>& GetAllRooms() const { return Rooms; }

	// Returns all room instances of the provided room data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsFromData(const URoomData* Data, TArray<URoom*>& Rooms);

	// Returns all room instances of any of the provided room data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsFromDataList(const TArray<URoomData*>& Data, TArray<URoom*>& Rooms);

	// Returns a random room from all the dungeon
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	URoom* GetRandomRoom();

	// Returns the total number of room
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int32 Count() const { return Rooms.Num(); }

	// Return true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyRoomData(const URoomData* RoomData) const;

	// Return true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyOneRoomDataFrom(const TArray<URoomData*>& RoomDataList) const;

	// Return the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountRoomData(const URoomData* RoomData) const;

	// Return the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountTotalRoomData(const TArray<URoomData*>& RoomDataList) const;

	// Return true if a specific RoomData type is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph", meta = (AutoCreateRefTerm = "RoomType"))
	bool HasAlreadyRoomType(const TSubclassOf<URoomData>& RoomType) const;

	// Return true if at least one of the RoomData type from the list provided is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyOneRoomTypeFrom(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const;

	// Return the number of a specific RoomData type in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph", meta = (AutoCreateRefTerm = "RoomType"))
	int CountRoomType(const TSubclassOf<URoomData>& RoomType) const;

	// Return the total number of RoomData type in the dungeon from the list provided
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountTotalRoomType(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const;

	URoom* GetRoomAt(FIntVector RoomCell) const;
	URoom* GetRoomByIndex(int64 Index) const;

	// Returns in OutRooms all the rooms in the Distance from each InRooms and optionally apply Func on each rooms.
	// Distance is the number of room connection between 2 rooms, not the distance in any unit.
	static void TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*)> Func);

protected:
	void Clear();

	int CountRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const;
	void GetRoomsByPredicate(TArray<URoom*>& OutRooms, TFunction<bool(const URoom*)> Predicate) const;


private:
	UPROPERTY()
	TArray<URoom*> Rooms;
};
