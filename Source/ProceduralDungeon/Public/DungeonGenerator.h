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
class UDungeonGraph;

UENUM()
enum class EGenerationResult : uint8
{
	None,
	Error,
	Success
};

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

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:

	// Update the seed and call the generation on all clients
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dungeon Generator")
	void Generate();

	// Unload the current dungeon
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dungeon Generator")
	void Unload();

	// ===== Methods that should be overriden in blueprint =====

	// Return the RoomData you want as root of the dungeon generation
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose First Room"))
	URoomData* ChooseFirstRoomData();

	/** Return the RoomData that will be connected to the Current Room
	* @param CurrentRoom The room from wich the generator will connect the next room.
	* @param DoorData The door of the CurrentRoom on which the next room will be connected (its location in room units, its orientation and its type).
	* @param DoorIndex The index of the door used on the next room to connect to the CurrentRoom.
	* Use -1 for a random (compatible) door, or the door index from the RoomData door array (0 is the first door).
	* WARNING: If the RandomDoor boolean of the RoomData is checked, then it will be considered -1 whatever you set here.
	* @return The room data asset used to instantiate the new room instance (must not be null)
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Next Room", ReturnDisplayName="Room Data", AutoCreateRefTerm = "DoorIndex"))
	URoomData* ChooseNextRoomData(const URoomData* CurrentRoom, const FDoorDef& DoorData, int& DoorIndex);

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

	// Initialize the room instances during the generation step
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator")
	void InitializeDungeon(const class UDungeonGraph* Rooms);

	// ===== Optional events =====

	// Called once before anything else when generating a new dungeon.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Pre Generation"))
	void OnPreGeneration();

	// Called once after all the dungeon generation (even if failed).
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Post Generation"))
	void OnPostGeneration();

	// Called before trying to generate a new dungeon and each time IsValidDungeon return false.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Generation Init"))
	void OnGenerationInit();

	// Called when all dungeon generation tries are exhausted (IsValidDungeon always return false).
	// No dungeon had been generated.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Generation Failed"))
	void OnGenerationFailed();

	// Called each time a room is added in the dungeon (but not spawned yet).
	// Those rooms can be destroyed without loading them if the generation try is not valid.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "On Room Added"))
	void OnRoomAdded(const URoomData* NewRoom);

	// ===== Utility functions you can use in blueprint =====

	// Return true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyRoomData(URoomData* RoomData);

	// Return true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList);

	// Return the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountRoomData(URoomData* RoomData);

	// Return the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountTotalRoomData(TArray<URoomData*> RoomDataList);

	// Return true if a specific RoomData type is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyRoomType(TSubclassOf<URoomData> RoomType);

	// Return true if at least one of the RoomData type from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Return the number of a specific RoomData type in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountRoomType(TSubclassOf<URoomData> RoomType);

	// Return the total number of RoomData type in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Return a random RoomData from the array provided
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	URoomData* GetRandomRoomData(TArray<URoomData*> RoomDataArray);

	// Returns the current number of room in the generated dungeon.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DisplayName = "Room Count", CompactNodeTitle = "Room Count", DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int GetNbRoom();

	// Returns an array of room data with compatible at least one compatible door with the door data provided.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	void GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator")
	const FRandomStream& GetRandomStream() { return Random; }

	URoom* GetRoomByIndex(int64 Index) const;

	// ===== Events =====

	// Called once before anything else when generating a new dungeon.
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPreGenerationEvent;

	// Called once after all the dungeon generation (even if failed).
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPostGenerationEvent;

	// Called before trying to generate a new dungeon and each time IsValidDungeon return false.
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnGenerationInitEvent;

	// Called when all dungeon generation tries are exhausted (IsValidDungeon always return false).
	// No dungeon had been generated.
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnGenerationFailedEvent;

	// Called each time a room is added in the dungeon (but not spawned yet).
	// Those rooms can be destroyed without loading them if the generation try is not valid.
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FRoomEvent OnRoomAddedEvent;

private:
	// Create virtually the dungeon (no load nor initialization of room levels)
	UFUNCTION()
	void CreateDungeon();

	// Adds some new rooms linked to ParentRoom into Rooms list output
	// AddedRooms contains only the new rooms added to Rooms list
	// Returns true if the dungeon should keep adding new rooms
	bool AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms, TArray<URoom*>& Rooms);

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

	// Clear and refill octree from the dungeon graph
	void UpdateOctree();

	// Initialize the seed depending on the seed type setting
	void UpdateSeed();

	// ===== FSM =====

	void SetState(EGenerationState NewState);
	void OnStateBegin(EGenerationState State);
	void OnStateTick(EGenerationState State);
	void OnStateEnd(EGenerationState State);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	bool bUseGeneratorTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	EGenerationType GenerationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	ESeedType SeedType;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation", meta = (EditCondition = "SeedType==ESeedType::AutoIncrement", EditConditionHides, DisplayAfter = "Seed"))
	uint32 SeedIncrement;

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (CompactNodeTitle = "Seed"))
	int32 GetSeed();

	uint32 GetUniqueId() const { return UniqueId; }
	uint64 GetGeneration() const { return Generation; }

	inline bool UseGeneratorTransform() const { return bUseGeneratorTransform; }
	FVector GetDungeonOffset() const;
	FQuat GetDungeonRotation() const;

protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Dungeon Generator", meta = (DisplayName = "Rooms"))
	UDungeonGraph* Graph;

private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Procedural Generation", meta = (EditCondition = "SeedType!=ESeedType::Random", EditConditionHides))
	uint32 Seed;

	static uint32 GeneratorCount;
	static const uint32 MaxTry {500};
	static const uint32 MaxRoomTry {10};

	FRandomStream Random;

	UPROPERTY(Transient)
	TArray<class ADoor*> DoorList;

	EGenerationState CurrentState {EGenerationState::Idle};
	uint32 UniqueId;

	UPROPERTY(Replicated, Transient)
	uint64 Generation {0};

	// Set to avoid adding increment the seed after we've set manually the seed
	bool bShouldIncrement {false};

	// Occlusion culling system
	TUniquePtr<FDungeonOctree> Octree;
	TSet<URoom*> CurrentPlayerRooms;

#if WITH_EDITOR
	// Transient. Only used to detect when occlusion setting is changed.
	bool bWasOcclusionEnabled {false};

	// Transient. Only used to detect when occlusion distance is changed.
	uint32 PreviousOcclusionDistance {0};
#endif
};
