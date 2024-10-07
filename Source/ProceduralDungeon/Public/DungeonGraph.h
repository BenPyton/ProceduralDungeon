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

#include "ReplicableObject.h"
#include "Templates/SubclassOf.h"
#include "DungeonGraph.generated.h"

class URoom;
class URoomData;
class URoomCustomData;
class ADungeonGeneratorBase;

UENUM()
enum class EDungeonGraphState : uint8
{
	None,
	RoomListChanged,
	RequestGeneration,
	NbState
};

// Holds the generated dungeon.
// You can access the rooms using many functions.
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDungeonGraph : public UReplicableObject
{
	GENERATED_BODY()

	friend ADungeonGeneratorBase;

public:
	UDungeonGraph();
	void AddRoom(URoom* Room);
	void InitRooms();
	void Clear();

	bool HasRooms() const { return Rooms.Num() > 0; }
	bool IsDirty() const { return CurrentState != EDungeonGraphState::None; }
	bool IsRequestingGeneration() const { return CurrentState == EDungeonGraphState::RequestGeneration; }

	// Returns all rooms
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	const TArray<URoom*>& GetAllRooms() const { return Rooms; }

	// Returns all room instances of the provided room data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsFromData(const URoomData* Data, TArray<URoom*>& Rooms);

	// Returns all room instances of any of the provided room data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsFromDataList(const TArray<URoomData*>& Data, TArray<URoom*>& Rooms);

	// Returns the first found room instance of the provided room data
	// (no defined order, so could be any room of the dungeon)
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	const URoom* GetFirstRoomFromData(const URoomData* Data);

	// Returns all room instances having the provided custom data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph", meta = (AutoCreateRefTerm = "CustomData"))
	void GetAllRoomsWithCustomData(const TSubclassOf<URoomCustomData>& CustomData, TArray<URoom*>& Rooms);

	// Returns all room instances having ALL the provided custom data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsWithAllCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& Rooms);

	// Returns all room instances having at least one of the provided custom data
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	void GetAllRoomsWithAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& Rooms);

	// Returns a random room from an array of room
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	URoom* GetRandomRoom(const TArray<URoom*>& RoomList) const;

	// Returns the total number of room
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph", meta = (CompactNodeTitle = "Count"))
	int32 Count() const { return Rooms.Num(); }

	// Returns true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyRoomData(const URoomData* RoomData) const;

	// Returns true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyOneRoomDataFrom(const TArray<URoomData*>& RoomDataList) const;

	// Returns the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountRoomData(const URoomData* RoomData) const;

	// Returns the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountTotalRoomData(const TArray<URoomData*>& RoomDataList) const;

	// Returns true if a specific RoomData type is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph", meta = (AutoCreateRefTerm = "RoomType"))
	bool HasAlreadyRoomType(const TSubclassOf<URoomData>& RoomType) const;

	// Returns true if at least one of the RoomData type from the list provided is already in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	bool HasAlreadyOneRoomTypeFrom(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const;

	// Returns the number of a specific RoomData type in the dungeon
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph", meta = (AutoCreateRefTerm = "RoomType"))
	int CountRoomType(const TSubclassOf<URoomData>& RoomType) const;

	// Returns the total number of RoomData type in the dungeon from the list provided
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	int CountTotalRoomType(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const;

	// Returns wether a path is valid between 2 rooms (no locked room blocking the way)
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph", meta = (ReturnDisplayName = "Yes"))
	bool HasValidPath(const URoom* From, const URoom* To, bool IgnoreLockedRooms = false);

	// Returns the minimum number of connected rooms between A and B.
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph")
	int NumberOfRoomBetween(const URoom* A, const URoom* B, bool IgnoreLockedRooms = false);

	// Returns the path between A and B.
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Graph", meta = (ReturnDisplayName = "Has Path"))
	bool GetPathBetween(const URoom* A, const URoom* B, TArray<URoom*>& ResultPath, bool IgnoreLockedRooms = false);

	URoom* GetRoomAt(FIntVector RoomCell) const;
	URoom* GetRoomByIndex(int64 Index) const;

	// Returns in OutRooms all the rooms in the Distance from each InRooms and optionally apply Func on each rooms.
	// Distance is the number of room connection between 2 rooms, not the distance in any unit.
	static void TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*)> Func);

	static bool FindPath(const URoom* From, const URoom* To, TArray<const URoom*>* OutPath = nullptr, bool IgnoreLocked = false);

protected:
	int CountRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const;
	void GetRoomsByPredicate(TArray<URoom*>& OutRooms, TFunction<bool(const URoom*)> Predicate) const;
	const URoom* FindFirstRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const;

	//~ Begin UReplicableObject Interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void RegisterReplicableSubobjects(bool bRegister) override;
	//~ End UReplicableObject Interface

	// Sync Rooms and ReplicatedRooms arrays
	void SynchronizeRooms();

	bool AreRoomsLoaded(int32& NbRoomLoaded) const;
	bool AreRoomsUnloaded(int32& NbRoomUnloaded) const;
	bool AreRoomsInitialized(int32& NbRoomInitialized) const;
	bool AreRoomsReady() const;

	void RequestGeneration();
	void RequestUnload();

private:
	UPROPERTY(Transient)
	TArray<URoom*> Rooms;

	// This array is synchronized with the server
	// We keep it separated to be able to unload previous rooms on clients
	UPROPERTY(ReplicatedUsing = OnRep_Rooms, Transient)
	TArray<URoom*> ReplicatedRooms;

	UFUNCTION()
	void OnRep_Rooms();

	EDungeonGraphState CurrentState {EDungeonGraphState::None};
	TWeakObjectPtr<ADungeonGeneratorBase> Generator {nullptr};
};
