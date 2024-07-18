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

#pragma once

#include "ReplicableObject.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Math/GenericOctree.h" // for FBoxCenterAndExtent (required for UE5.0)
#include "Room.generated.h"

class ADungeonGenerator;
class ARoomLevel;
class URoomData;
class ADoor;
class URoomCustomData;
class ULevelStreamingDynamic;

USTRUCT()
struct FRoomConnection
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TWeakObjectPtr<URoom> OtherRoom {nullptr};

	UPROPERTY()
	int OtherDoorIndex {-1};

	UPROPERTY()
	ADoor* DoorInstance {nullptr};
};

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
class PROCEDURALDUNGEON_API URoom : public UReplicableObject
{
	GENERATED_BODY()

public:
	// TODO: Make them private
	UPROPERTY()
	ULevelStreamingDynamic* Instance {nullptr};
	UPROPERTY(Replicated)
	FIntVector Position {0};
	UPROPERTY(Replicated)
	EDoorDirection Direction {EDoorDirection::NbDirection};

	URoom();

	// Returns the room data asset of this room instance.
	UFUNCTION(BlueprintPure, Category = "Room")
	const URoomData* GetRoomData() const { return RoomData; }

	const ADungeonGenerator* Generator() const { return GeneratorOwner.Get(); }
	void SetPlayerInside(bool PlayerInside);
	void SetVisible(bool Visible);
	FORCEINLINE uint64 GetRoomID() const { return Id; }
	FORCEINLINE bool IsReady() const { return RoomData != nullptr; }

	// Is the player currently inside the room?
	// A player can be in multiple rooms at once, for example when he stands at the door frame,
	// the player's capsule is in both rooms.
	UFUNCTION(BlueprintPure, Category = "Room")
	FORCEINLINE bool IsPlayerInside() const { return bPlayerInside; }

	// Is the room currently visible?
	UFUNCTION(BlueprintPure, Category = "Room", meta = (CompactNodeTitle = "Is Visible"))
	FORCEINLINE bool IsVisible() const { return bIsVisible; }

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
	bool GetCustomData(const TSubclassOf<URoomCustomData>& DataType, URoomCustomData*& Data) const;
	bool HasCustomData(const TSubclassOf<URoomCustomData>& DataType) const;

	// Returns the RandomStream from the Dungeon Generator
	UFUNCTION(BlueprintCallable, Category = "Room")
	FRandomStream GetRandomStream() const;

	// Get the door actor from a specific index.
	// @param DoorIndex The index of the door to retrieve.
	// @return The door actor at the index, or null if the index is out of range.
	UFUNCTION(BlueprintCallable, Category = "Room")
	ADoor* GetDoor(int DoorIndex) const;

	// Fill an array with all the door actors connected to the room.
	// @param OutDoors THIS IS NOT AN INPUT! This array will be emptied and then filled with the door actors. This is your result!
	UFUNCTION(BlueprintPure = false, Category = "Room")
	void GetAllDoors(UPARAM(ref) TArray<ADoor*>& OutDoors) const;

	// Returns true if the door at DoorIndex is connected to another room.
	// @param DoorIndex The index of the door to check.
	UFUNCTION(BlueprintPure, Category = "Room")
	bool IsDoorConnected(int DoorIndex) const;

	// Returns true if all the doors of this room are connected to other rooms.
	UFUNCTION(BlueprintPure, Category = "Room")
	bool AreAllDoorsConnected() const;

	// Returns the connected room instance at DoorIndex.
	UFUNCTION(BlueprintPure, Category = "Room")
	URoom* GetConnectedRoomAt(int DoorIndex) const;

	// Returns all the room instances connected with this one.
	// @param ConnectedRooms This array will be filled with the room instances.
	UFUNCTION(BlueprintPure, Category = "Room")
	void GetAllConnectedRooms(TArray<URoom*>& ConnectedRooms) const;

	// Returns the world center position of the room.
	UFUNCTION(BlueprintPure, Category = "Room")
	FVector GetBoundsCenter() const;

	// Returns the world size of the room.
	UFUNCTION(BlueprintPure, Category = "Room")
	FVector GetBoundsExtent() const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_RoomData)
	URoomData* RoomData {nullptr};

	UPROPERTY(Replicated, Transient)
	TArray<FCustomDataPair> CustomData;

	UPROPERTY(ReplicatedUsing = OnRep_Connections)
	TArray<FRoomConnection> Connections;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ADungeonGenerator> GeneratorOwner {nullptr};

	UPROPERTY(ReplicatedUsing = OnRep_Id)
	int64 Id {-1};

	bool bPlayerInside {false};
	bool bIsVisible {true};

	UPROPERTY(ReplicatedUsing = OnRep_IsLocked)
	bool bIsLocked {false};

	const FCustomDataPair* GetDataPair(const TSubclassOf<URoomCustomData>& DataType) const;

protected:
	//~ Begin UReplicableObject Interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void RegisterReplicableSubobjects(bool bRegister) override;
	//~ End UReplicableObject Interface

	void SetPosition(const FIntVector& NewPosition);
	void SetDirection(EDoorDirection NewDirection);

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
	void Init(URoomData* RoomData, ADungeonGenerator* Generator, int32 RoomId);

	bool IsConnected(int Index) const;
	void SetConnection(int Index, URoom* Room, int OtherDoorIndex);
	TWeakObjectPtr<URoom> GetConnection(int Index) const;
	int GetFirstEmptyConnection() const;

	void Instantiate(UWorld* World);
	void Destroy();
	ARoomLevel* GetLevelScript() const;
	bool IsInstanceLoaded() const;
	bool IsInstanceUnloaded() const;
	bool IsInstanceInitialized() const;

	EDoorDirection GetDoorWorldOrientation(int DoorIndex);
	FIntVector GetDoorWorldPosition(int DoorIndex);
	int GetConnectionCount() const { return Connections.Num(); }
	int GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsDoorInstanced(int DoorIndex);
	void SetDoorInstance(int DoorIndex, ADoor* Door);
	int GetOtherDoorIndex(int DoorIndex);
	void TryConnectToExistingDoors(TArray<URoom*>& RoomList);

	FIntVector WorldToRoom(const FIntVector& WorldPos) const;
	FIntVector RoomToWorld(const FIntVector& RoomPos) const;
	EDoorDirection WorldToRoom(const EDoorDirection& WorldRot) const;
	EDoorDirection RoomToWorld(const EDoorDirection& RoomRot) const;
	FBoxMinAndMax WorldToRoom(const FBoxMinAndMax& WorldBox) const;
	FBoxMinAndMax RoomToWorld(const FBoxMinAndMax& RoomBox) const;
	void SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot);
	void SetPositionFromDoor(int DoorIndex, FIntVector WorldPos);
	void SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsOccupied(FIntVector Cell);

	FTransform GetTransform() const;
	FBoxCenterAndExtent GetBounds() const;
	FBoxCenterAndExtent GetLocalBounds() const;
	FBoxMinAndMax GetIntBounds() const;

	// AABB Overlapping
	static bool Overlap(const URoom& A, const URoom& B);
	static bool Overlap(const URoom& Room, const TArray<URoom*>& RoomList);

	static void Connect(URoom& RoomA, int DoorA, URoom& RoomB, int DoorB);
	static URoom* GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList);

private:
	// Utility functions to load/unload level instances
	static ULevelStreamingDynamic* LoadInstance(UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& Level, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation);
	static void UnloadInstance(ULevelStreamingDynamic* Instance);
};
