/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "DungeonOctree.h"
#include "ProceduralDungeonTypes.h"
#include "DungeonGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenerationEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomEvent, const URoomData*, NewRoom);

class ADoor;
class URoom;
class UDoorType;

UCLASS(Blueprintable, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:

	// Update the seed and call the generation on all clients
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dungeon Generator")
	void Generate();

	// ===== Methods that should be overriden in blueprint =====

	// Return the RoomData you want as root of the dungeon generation
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose First Room"))
	URoomData* ChooseFirstRoomData();

	// Return the RoomData that will be connected to the Current Room
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Next Room"))
	URoomData* ChooseNextRoomData(const URoomData* CurrentRoom, const FDoorDef& DoorData);

	// Return the door which will be spawned between Current Room and Next Room
	// Use IsDoorOfType function to compare a door class with DoorType.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Door"))
	TSubclassOf<ADoor> ChooseDoor(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType);

	// Condition to validate a dungeon Generation
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Is Valid Dungeon"))
	bool IsValidDungeon();

	// Condition to continue or stop adding room to the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Continue To Add Room"))
	bool ContinueToAddRoom();

	// ===== Optional events =====

	// Called after unloading previous dungeon but before generating next dungeon.
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "Pre Generation"))
	void OnPreGeneration_BP();

	// Called after all rooms are loaded and initialized
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "Post Generation"))
	void OnPostGeneration_BP();

	// Called before generating a new dungeon and each time IsValidDungeon return false
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "Generation Init"))
	void OnGenerationInit_BP();

	// Called when dungeon failed to generate a valid dungeon after exhausting all tries
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "Generation Failed"))
	void OnGenerationFailed_BP();

	// Called when the room NewRoom is added in the generation (but not spawned yet)
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "On Room Added"))
	void OnRoomAdded_BP(const URoomData* NewRoom);

	// ===== Utility functions you can use in blueprint =====

	// Return true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	bool HasAlreadyRoomData(URoomData* RoomData);

	// Return true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	bool HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList);

	// Return the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	int CountRoomData(URoomData* RoomData);

	// Return the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	int CountTotalRoomData(TArray<URoomData*> RoomDataList);

	// Return true if a specific RoomData type is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	bool HasAlreadyRoomType(TSubclassOf<URoomData> RoomType);

	// Return true if at least one of the RoomData type from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	bool HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Return the number of a specific RoomData type in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	int CountRoomType(TSubclassOf<URoomData> RoomType);

	// Return the total number of RoomData type in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	int CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Return a random RoomData from the array provided
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	URoomData* GetRandomRoomData(TArray<URoomData*> RoomDataArray);

	// Returns the current number of room in the generated dungeon.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DisplayName = "Room Count", CompactNodeTitle = "Room Count"))
	int GetNbRoom() { return RoomList.Num(); }

	// Returns an array of room data with compatible at least one compatible door with the door data provided.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	void GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData);

	URoom* GetRoomAt(FIntVector RoomCell);
	URoom* GetRoomByIndex(int64 Index);

	// ===== Events =====

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPreGenerationEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPostGenerationEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnGenerationInitEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnGenerationFailedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FRoomEvent OnRoomAddedEvent;

protected:

	// ===== Implementation of blueprint native events  =====

	UFUNCTION()
	virtual URoomData* ChooseFirstRoomData_Implementation();

	UFUNCTION()
	virtual URoomData* ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const FDoorDef& DoorData);

	UFUNCTION()
	virtual TSubclassOf<ADoor> ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType);

	UFUNCTION()
	virtual bool IsValidDungeon_Implementation();

	UFUNCTION()
	virtual bool ContinueToAddRoom_Implementation();

	// ===== Overridable events by native inheritance =====

	UFUNCTION()
	virtual void OnPreGeneration() {}

	UFUNCTION()
	virtual void OnPostGeneration() {}

	UFUNCTION()
	virtual void OnGenerationInit() {}

	UFUNCTION()
	virtual void OnGenerationFailed() {}

	UFUNCTION()
	virtual void OnRoomAdded(const URoomData* NewRoom) {}

private:
	// Launch the generation process of the dungeon
	UFUNCTION(NetMulticast, Reliable)
	void BeginGeneration(uint32 GenerationSeed);

	// Create virtually the dungeon (no load nor initialization of rooms)
	UFUNCTION()
	void CreateDungeon();

	// That add a room function to generate all rooms
	TArray<URoom*> AddNewRooms(URoom& ParentRoom);

	// Instantiate a room in the scene
	void InstantiateRoom(URoom* Room);

	// Load all room levels
	void LoadAllRooms();

	// unload all room levels
	void UnloadAllRooms();

	// Update the rooms visibility based on the player position
	void UpdateRoomVisibility();

	// Reset all data from a specific generation
	void Reset();

	// ===== FSM =====

	void SetState(EGenerationState NewState);
	void OnStateBegin(EGenerationState State);
	void OnStateTick(EGenerationState State);
	void OnStateEnd(EGenerationState State);

	// ===== Dispatch optional events =====

	void DispatchPreGeneration();
	void DispatchPostGeneration();
	void DispatchGenerationInit();
	void DispatchGenerationFailed();
	void DispatchRoomAdded(const URoomData* NewRoom);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	EGenerationType GenerationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	ESeedType SeedType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	bool bUseGeneratorTransform;

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (CompactNodeTitle = "Seed"))
	int32 GetSeed();

	int32 GetUniqueId() const { return UniqueId; }

	inline bool UseGeneratorTransform() const { return bUseGeneratorTransform; }
	FVector GetDungeonOffset() const;
	FQuat GetDungeonRotation() const;

private:
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	uint32 Seed;

	static int32 GeneratorCount;
	static const int MaxTry = 500;
	static const int MaxRoomTry = 10;
	FRandomStream Random;

	UPROPERTY()
	TArray<URoom*> RoomList;
	UPROPERTY()
	TArray<class ADoor*> DoorList;

	EGenerationState CurrentState = EGenerationState::None;
	int32 UniqueId;

	// Occlusion culling system
	TUniquePtr<FDungeonOctree> Octree;
	TSet<URoom*> CurrentPlayerRooms;
};
