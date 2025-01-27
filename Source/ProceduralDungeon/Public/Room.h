/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Benoit Pelletier
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
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Math/GenericOctree.h" // for FBoxCenterAndExtent (required for UE5.0)
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "UObject/SoftObjectPtr.h"
#include "RoomData.h" // for TSoftObjectPtr to compile. @TODO: Would be great to find a way to not include it
#include "ReadOnlyRoom.h"
#include "VoxelBounds/VoxelBounds.h"
#include "Room.generated.h"

class ADungeonGeneratorBase;
class ARoomLevel;
class ADoor;
class URoomCustomData;
class ULevelStreamingDynamic;

// I made this struct instead of a map to allow replication over network.
USTRUCT()
struct FCustomDataPair
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UClass* DataClass {nullptr};
	UPROPERTY()
	URoomCustomData* Data {nullptr};
};

// The room instances of the dungeon.
// Holds data specific to each room instance, e.g. location, direction, is player inside, room custom data, etc.
UCLASS(BlueprintType, meta = (ShortToolTip = "The room instances of the dungeon."))
class PROCEDURALDUNGEON_API URoom : public UReplicableObject, public IReadOnlyRoom, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	// TODO: Make them private
	UPROPERTY()
	ULevelStreamingDynamic* Instance {nullptr};
	UPROPERTY(Replicated, SaveGame)
	FIntVector Position {0};
	UPROPERTY(Replicated, SaveGame)
	EDoorDirection Direction {EDoorDirection::NbDirection};

	//~ Begin IReadOnlyRoom Interface
	virtual const URoomData* GetRoomData() const override { return RoomData.Get(); }
	virtual int64 GetRoomID() const override{ return Id; }
	virtual FIntVector GetPosition() const { return Position; }
	virtual EDoorDirection GetDirection() const { return Direction; }
	virtual bool AreAllDoorsConnected() const override;
	virtual int CountConnectedDoors() const override;
	virtual FVector GetBoundsCenter() const override;
	virtual FVector GetBoundsExtent() const override;
	//~ End IReadOnlyRoom Interface

	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override;
	virtual bool FixupReferences(UObject* Context) override;
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PreSaveDungeon_Implementation() override;
	virtual void PostLoadDungeon_Implementation() override;
	//~ End IDungeonSaveInterface Interface

	const ADungeonGeneratorBase* Generator() const { return GeneratorOwner.Get(); }
	void SetPlayerInside(bool PlayerInside);
	void SetVisible(bool Visible);
	FORCEINLINE bool IsReady() const { return RoomData != nullptr; }

	// Is the player currently inside the room?
	// A player can be in multiple rooms at once, for example when he stands at the door frame,
	// the player's capsule is in both rooms.
	UFUNCTION(BlueprintPure, Category = "Room")
	FORCEINLINE bool IsPlayerInside() const { return bPlayerInside; }

	// Is the room currently visible?
	UFUNCTION(BlueprintPure, Category = "Room", meta = (CompactNodeTitle = "Is Visible"))
	FORCEINLINE bool IsVisible() const { return bIsVisible || bForceVisible; }

	// Force the room to be veisible
	UFUNCTION(BlueprintCallable, Category = "Room")
	void ForceVisibility(bool bForce);

	// Is the room locked?
	// If it is, the doors will be locked (except if they have `Alway Unlocked`).
	UFUNCTION(BlueprintPure, Category = "Room", meta = (CompactNodeTitle = "Is Locked"))
	FORCEINLINE bool IsLocked() const { return bIsLocked; }

	// Lock or unlock the room instance.
	// Will lock/unlock the doors too (except if they have `Alway Unlocked`).
	// @param lock Should the room be locked?
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Room")
	void Lock(bool lock);

	// Access to custom data of the room.
	// @param DataType The type of the custom data to retrieve.
	// @param Data The custom data found, or null if no custom data found.
	// @return True if a custom data of the specified type has been found, false otherwise.
	UFUNCTION(BlueprintCallable, Category = "Room", meta = (DisplayName = "Get Custom Data", ExpandBoolAsExecs = "ReturnValue", DeterminesOutputType = "DataType", DynamicOutputParam = "Data"))
	bool GetCustomData_BP(TSubclassOf<URoomCustomData> DataType, URoomCustomData*& Data);

	// Check if the room instance contains a custom data of a specific type.
	// @param DataType The type of the custom data to check.
	// @return True if the rooms has a custom data of the specified type, false otherwise.
	UFUNCTION(BlueprintCallable, Category = "Room", meta = (DisplayName = "Has Custom Data", ExpandBoolAsExecs = "ReturnValue", AutoCreateRefTerm = "DataType"))
	bool HasCustomData_BP(const TSubclassOf<URoomCustomData>& DataType);

	bool CreateCustomData(const TSubclassOf<URoomCustomData>& DataType);
	bool CreateAllCustomData();
	bool GetCustomData(const TSubclassOf<URoomCustomData>& DataType, URoomCustomData*& Data) const;
	bool HasCustomData(const TSubclassOf<URoomCustomData>& DataType) const;

	// Returns the RandomStream from the Dungeon Generator
	// [DEPRECATED] Use a DeterministicRandom component on actors instead.
	UFUNCTION(BlueprintCallable, Category = "Room", meta = (DeprecatedFunction, DeprecationMessage = "Use a DeterministicRandom component on actors instead."))
	FRandomStream GetRandomStream() const;

	// Get the door actor from a specific index.
	// @param DoorIndex The index of the door to retrieve.
	// @return The door actor at the index, or null if the index is out of range.
	UFUNCTION(BlueprintCallable, Category = "Room")
	ADoor* GetDoor(int32 DoorIndex) const;

	// Fill an array with all the door actors connected to the room.
	// @param OutDoors THIS IS NOT AN INPUT! This array will be emptied and then filled with the door actors. This is your result!
	UFUNCTION(BlueprintPure = false, Category = "Room")
	void GetAllDoors(UPARAM(ref) TArray<ADoor*>& OutDoors) const;

	// Returns true if the door at DoorIndex is connected to another room.
	// @param DoorIndex The index of the door to check.
	UFUNCTION(BlueprintPure, Category = "Room")
	bool IsDoorConnected(int DoorIndex) const;

	// Returns the connected room instance at DoorIndex.
	UFUNCTION(BlueprintPure, Category = "Room")
	URoom* GetConnectedRoomAt(int DoorIndex) const;

	// Returns all the room instances connected with this one.
	// @param ConnectedRooms This array will be filled with the room instances.
	UFUNCTION(BlueprintPure, Category = "Room")
	void GetAllConnectedRooms(TArray<URoom*>& ConnectedRooms) const;

	// Returns the index of the provided room, or -1 if room is not connected.
	UFUNCTION(BlueprintPure, Category = "Room")
	int32 GetConnectedRoomIndex(const URoom* OtherRoom) const;

	// Returns the door actor shared with the provided room.
	// Returns null if the provided room is not connected with this.
	UFUNCTION(BlueprintPure, Category = "Room")
	void GetDoorsWith(const URoom* OtherRoom, TArray<ADoor*>& Doors) const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_RoomData, SaveGame)
	TSoftObjectPtr<URoomData> RoomData {nullptr};

	UPROPERTY(Replicated, Transient)
	TArray<FCustomDataPair> CustomData;

	UPROPERTY(ReplicatedUsing = OnRep_Connections)
	TArray<TWeakObjectPtr<class URoomConnection>> Connections;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ADungeonGeneratorBase> GeneratorOwner {nullptr};

	UPROPERTY(ReplicatedUsing = OnRep_Id, SaveGame)
	int64 Id {-1};

	bool bPlayerInside {false};
	bool bIsVisible {true};
	bool bForceVisible {false};

	UPROPERTY(ReplicatedUsing = OnRep_IsLocked, SaveGame)
	bool bIsLocked {false};

	const FCustomDataPair* GetDataPair(const TSubclassOf<URoomCustomData>& DataType) const;

protected:
	//~ Begin UReplicableObject Interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void RegisterReplicableSubobjects(bool bRegister) override;
	//~ End UReplicableObject Interface

	void SetPosition(const FIntVector& NewPosition);
	void SetDirection(EDoorDirection NewDirection);
	void UpdateVisibility() const;

	UFUNCTION() // Needed macro for replication to work
	void OnRep_RoomData();

	UFUNCTION() // Needed macro for replication to work
	void OnRep_Id();

	UFUNCTION() // Needed macro for replication to work
	void OnRep_Connections();

	UFUNCTION() // Needed macro for replication to work
	void OnRep_IsLocked();

	UFUNCTION() // needed macro for binding to delegate
	void OnInstanceLoaded();

public:
	void Init(URoomData* RoomData, ADungeonGeneratorBase* Generator, int32 RoomId);

	void Instantiate(UWorld* World);
	void Destroy();
	ARoomLevel* GetLevelScript() const;
	bool IsInstanceLoaded() const;
	bool IsInstanceUnloaded() const;
	bool IsInstanceInitialized() const;
	void CreateLevelComponents(ARoomLevel* LevelActor);

	EDoorDirection GetDoorWorldOrientation(int DoorIndex) const;
	FIntVector GetDoorWorldPosition(int DoorIndex) const;

	int32 GetConnectionCount() const { return Connections.Num(); }
	bool IsConnected(int32 DoorIndex) const;
	void SetConnection(int32 DoorIndex, URoomConnection* Conn);
	TWeakObjectPtr<URoom> GetConnectedRoom(int32 DoorIndex) const;
	int32 GetFirstEmptyConnection() const;
	void GetAllEmptyConnections(TArray<int32>& EmptyConnections) const;

	bool IsDoorIndexValid(int32 DoorIndex) const;
	int32 GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot) const;
	int32 GetOtherDoorIndex(int32 DoorIndex) const;
	const FDoorDef& GetDoorDef(int32 DoorIndex) const;
	const FDoorDef& GetDoorDefAt(FIntVector WorldPos, EDoorDirection WorldRot) const;

	FIntVector WorldToRoom(const FIntVector& WorldPos) const;
	FIntVector RoomToWorld(const FIntVector& RoomPos) const;
	EDoorDirection WorldToRoom(const EDoorDirection& WorldRot) const;
	EDoorDirection RoomToWorld(const EDoorDirection& RoomRot) const;
	FBoxMinAndMax WorldToRoom(const FBoxMinAndMax& WorldBox) const;
	FBoxMinAndMax RoomToWorld(const FBoxMinAndMax& RoomBox) const;
	FDoorDef WorldToRoom(const FDoorDef& WorldDoor) const;
	FDoorDef RoomToWorld(const FDoorDef& RoomDoor) const;
	FVoxelBounds WorldToRoom(const FVoxelBounds& WorldBounds) const;
	FVoxelBounds RoomToWorld(const FVoxelBounds& RoomBounds) const;
	void SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot);
	void SetPositionFromDoor(int DoorIndex, FIntVector WorldPos);
	void SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsOccupied(FIntVector Cell);

	FTransform GetTransform() const;
	FBoxCenterAndExtent GetBounds() const;
	FBoxCenterAndExtent GetLocalBounds() const;
	FBoxMinAndMax GetIntBounds() const;
	FVoxelBounds GetVoxelBounds() const;

	// AABB Overlapping
	static bool Overlap(const URoom& A, const URoom& B);
	static bool Overlap(const URoom& Room, const TArray<URoom*>& RoomList);

	static URoom* GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList);

private:
	// Utility functions to load/unload level instances
	static ULevelStreamingDynamic* LoadInstance(UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& Level, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation);
	static void UnloadInstance(ULevelStreamingDynamic* Instance);

private:
	using FActorSaveDataMap = TMap<FGuid, TArray<uint8>>;

	// This struct holds the data applied at later stages of the loading process.
	// For example, it holds the connection indices, that will be used later to resolve the connection references.
	struct FSaveData
	{
		TArray<int32> ConnectionIds;
		TArray<uint8> LevelActor;
		FActorSaveDataMap Actors;
	};

	// This is a unique ptr so we have a data only when we need it.
	TUniquePtr<FSaveData> SaveData {nullptr};

	bool SerializeLevelActors(FSaveData& Data, bool bIsLoading);
	void DispatchCallbackToSavedLevelActors(TFunction<void(AActor*)> Callback) const;
};
