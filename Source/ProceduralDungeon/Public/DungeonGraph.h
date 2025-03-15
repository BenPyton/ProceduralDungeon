/*
 * MIT License
 *
 * Copyright (c) 2023-2025 Benoit Pelletier
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
#include "Interfaces/RoomContainer.h"
#include "Interfaces/GeneratorProvider.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Templates/SubclassOf.h"
#include "Templates/Function.h"
#include "ProceduralDungeonTypes.h"
#include "DungeonGraph.generated.h"

class URoom;
class URoomData;
class URoomCustomData;
class URoomConnection;
class ADungeonGeneratorBase;

// Holds the generated dungeon.
// You can access the rooms using many functions.
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDungeonGraph : public UReplicableObject, public IRoomContainer, public IGeneratorProvider, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

	friend ADungeonGeneratorBase;

#if WITH_DEV_AUTOMATION_TESTS
	friend class FDungeonGraphTest;
#endif

public:
	//~ Begin IRoomContainer Interface
	// Returns the room instance with the provided index.
	// Returns null if no room exists with the provided index.
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	virtual URoom* GetRoomByIndex(int64 Index) const final;

	virtual URoomConnection* GetConnectionByIndex(int32 Index) const override;
	//~ End IRoomContainer Interface

	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override;
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PostLoadDungeon_Implementation() override;
	//~ End IDungeonSaveInterface Interface

	//~ Begin IGeneratorProvider Interface
	virtual ADungeonGeneratorBase* GetGenerator() const override { return Generator.Get(); }
	//~ End IGeneratorProvider Interface

	void AddRoom(URoom* Room);
	void InitRooms();
	void Clear();

	bool TryConnectDoor(URoom* Room, int32 DoorIndex);
	bool TryConnectToExistingDoors(URoom* Room);

	bool HasRooms() const { return Rooms.Num() > 0; }
	bool IsDirty() const { return bIsDirty; }

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
	UFUNCTION(BlueprintPure = false, Category = "Dungeon Graph", meta = (ReturnDisplayName = "Yes"))
	bool HasValidPath(const URoom* From, const URoom* To, bool IgnoreLockedRooms = false) const;

	// Returns the minimum number of connected rooms between A and B.
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintPure = false, Category = "Dungeon Graph")
	int32 NumberOfRoomBetween(const URoom* A, const URoom* B, bool IgnoreLockedRooms = false) const;

	// Returns the minimum number of connected rooms between A and B.
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintPure = false, Category = "Dungeon Graph", meta = (DisplayName = "Number Of Room Between (using ReadOnlyRoom)"))
	int32 NumberOfRoomBetween_ReadOnly(TScriptInterface<IReadOnlyRoom> A, TScriptInterface<IReadOnlyRoom> B) const;

	// Returns the path between A and B.
	// Note: Could be pure, but since it can be heavy duty for large dungeons, keep it impure to avoid duplicate calls.
	UFUNCTION(BlueprintPure = false, Category = "Dungeon Graph", meta = (ReturnDisplayName = "Has Path"))
	bool GetPathBetween(const URoom* A, const URoom* B, TArray<URoom*>& ResultPath, bool IgnoreLockedRooms = false) const;

	// Returns the room instance at the provided room cell (expressed in Room Units, not Unreal Units!!!).
	// Returns null if no room located at the provided cell.
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	URoom* GetRoomAt(FIntVector RoomCell) const;

	// Returns the center of the bounding box of the dungeon.
	// @see GetDungeonBoundsExtents
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	FVector GetDungeonBoundsCenter() const;

	// Returns the extent (half size) of the bounding box of the dungeon.
	// @see GetDungeonBoundsCenter
	UFUNCTION(BlueprintPure, Category = "Dungeon Graph")
	FVector GetDungeonBoundsExtent() const;

	// Returns the computed dungeon bounds.
	class FBoxCenterAndExtent GetDungeonBounds(const FTransform& Transform = FTransform::Identity) const;
	struct FBoxMinAndMax GetIntBounds() const;

	// Returns in OutRooms all the rooms in the Distance from each InRooms and optionally apply Func on each rooms.
	// Distance is the number of room connection between 2 rooms, not the distance in any unit.
	static void TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*)> Func);

	static bool FindPath(const URoom* From, const URoom* To, TArray<const URoom*>* OutPath = nullptr, bool IgnoreLocked = false);

	const TArray<URoomConnection*>& GetAllConnections() const { return RoomConnections; }

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

	// Replace existing room array from the one loaded in saved data.
	// This does nothing if there is no data loaded from a saved dungeon.
	void RetrieveRoomsFromLoadedData();

	// Create and store a new connection between two rooms in RoomConnections.
	void Connect(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB);

	bool AreRoomsLoaded(int32& NbRoomLoaded) const;
	bool AreRoomsUnloaded(int32& NbRoomUnloaded) const;
	bool AreRoomsInitialized(int32& NbRoomInitialized) const;
	bool AreRoomsReady() const;

	void SpawnAllDoors();
	void LoadAllRooms();
	void UnloadAllRooms();

	void MarkDirty() { bIsDirty = true; }

	// Extends the bounds if necessary to include the provided room.
	void UpdateBounds(const URoom* Room);

	// Recreate the bounds using the whole room list.
	void RebuildBounds();

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<URoom>> Rooms;

	UPROPERTY(Replicated, Transient)
	TArray<TObjectPtr<URoomConnection>> RoomConnections;

	// This array is synchronized with the server
	// We keep it separated to be able to unload previous rooms on clients
	UPROPERTY(ReplicatedUsing = OnRep_Rooms, Transient)
	TArray<TObjectPtr<URoom>> ReplicatedRooms;

	UFUNCTION()
	void OnRep_Rooms();

	bool bIsDirty {false};

	// @TODO: Make something to decouple the ADungeonGenerator from the UDungeonGraph.
	// It is currently used only to get its random stream in the `Get Random Room` function.
	// We could instead either:
	// - Use an interface that provides a random stream => good way to not induce breaking changes in the code.
	// - Pass the random stream as an input to that function => will need to make some changes in existing projects.
	TWeakObjectPtr<ADungeonGeneratorBase> Generator {nullptr};

	// Transient. The computed bounds of the dungeon. Updated each time the room list changes.
	FBoxMinAndMax Bounds;

private:
	struct FSaveData
	{
		TArray<URoom*> Rooms;
		TArray<URoomConnection*> Connections;
	};

	TUniquePtr<FSaveData> SavedData {nullptr};
};
