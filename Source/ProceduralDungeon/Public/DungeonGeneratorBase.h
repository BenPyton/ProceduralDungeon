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

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "DungeonOctree.h"
#include "ProceduralDungeonTypes.h"
#include "CollisionQueryParams.h"
#include "UObject/ScriptInterface.h"
#include "DungeonGeneratorBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenerationEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomEvent, const URoomData*, Room, const TScriptInterface<IReadOnlyRoom>&, RoomInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomDoorEvent, const URoomData*, Room, const FDoorDef&, Door);

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

// This is the main actor of the plugin. The dungeon generator is responsible to generate dungeons and replicate them over the network. 
UCLASS(Abstract, NotBlueprintable, BlueprintType, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ADungeonGeneratorBase : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGeneratorBase();

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~ End AActor Interface

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

	// Return the door which will be spawned between Current Room and Next Room
	// @param CurrentRoom The first of both rooms to have been generated. By default the door will face this room.
	// @param NextRoom The second of both rooms to have been generated. Set Flipped to true to make the door facing this room.
	// @param DoorType The door type set by both room data. Use IsDoorOfType function to compare a door actor class with this.
	// @param Flipped Tells which room the door is facing between CurrentRoom (false) and NextRoom (true).
	// @return The door actor class to spawn between CurrentRoom and NextRoom.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Door"))
	TSubclassOf<ADoor> ChooseDoor(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped);

	// ===== Optional functions to override =====

	// Initialize the room instances during the generation step
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator")
	void InitializeDungeon(const UDungeonGraph* Rooms);

	// Returns which pawn is used for the room culling system.
	// This pawn will also affect the PlayerInside variable of the rooms.
	// By default returns GetPlayerController(0)->GetPawnOrSpectator().
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator")
	APawn* GetVisibilityPawn();

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
	// @param NewRoom The room data successfully added to the dungeon [DEPRECATED: will be removed in future version, use RoomInstance->GetRoomData instead]
	// @param RoomInstance The room successfully added to the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "On Room Added"))
	void OnRoomAdded(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance);

	// Called each time no room could have been placed at a door (all room placement tries have been exhausted).
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Failed To Add Room"))
	void OnFailedToAddRoom(const URoomData* FromRoom, const FDoorDef& FromDoor);

	// ===== Utility functions you can use in blueprint =====

	// Return a random RoomData from the array provided
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	URoomData* GetRandomRoomData(TArray<URoomData*> RoomDataArray);

	// Return a random RoomData from the weighted map provided.
	// For example: you have RoomA with weight 1 and RoomB with weight 2,
	// then RoomA has proba of 1/3 and RoomB 2/3 to be returned.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	URoomData* GetRandomRoomDataWeighted(const TMap<URoomData*, int>& RoomDataWeightedMap);

	// Returns an array of room data with at least one compatible door with the door data provided.
	// @param bSuccess True if at least one compatible room data was found.
	// @param CompatibleRooms Filled with all compatible room data found.
	// @param RoomDataArray The list of room data to check for compatibility.
	// @param DoorData The door used to check if a room is compatible.
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator")
	void GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData);

	// Access to the random stream of the procedural dungeon. You should always use this for the procedural generation.
	// @return The random stream used by the dungeon generator.
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator")
	const FRandomStream& GetRandomStream() { return Random; }

	// Returns the current generation progress.
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator")
	float GetProgress() const;

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

	// Called each time no room could have been placed at a door (all room placement tries have been exhausted).
	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FRoomDoorEvent OnFailedToAddRoomEvent;

protected:
	// Create virtually the dungeon (no load nor initialization of room levels)
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (BlueprintInternalUseOnly = true))
	bool CreateDungeon();

	// ===== Functions for dungeon creation =====
	// @TODO: For now, I didn't found a way to hide them on child blueprints (HideFunctions and KismetHideOverrides do not work)
	// So in the meantime I marked them as BlueprintInternalUseOnly.
	// Can still be used in C++.

	// Clear current graph and call GenerationInit event.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator", meta = (BlueprintInternalUseOnly = true))
	void StartNewDungeon();

	// Initialize room instances after all rooms have been placed and connected (call InitializeDungeon).
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator", meta = (BlueprintInternalUseOnly = true))
	void FinalizeDungeon();

	// Create and initialize a new room instance using the room data provided.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Dungeon Generator", meta = (BlueprintInternalUseOnly = true))
	URoom* CreateRoomInstance(URoomData* RoomData);

	// Set the position and rotation of a room instance and return true if there is nothing colliding with it.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Dungeon Generator", meta = (ReturnDisplayName = "Success", HidePin = "World", BlueprintInternalUseOnly = true))
	bool TryPlaceRoom(URoom* const& Room, int DoorIndex, const FDoorDef& TargetDoor, const UWorld* World = nullptr) const;

	// Finalize the room creation by adding it to the dungeon graph. OnRoomAdded is called here.
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator", meta = (ReturnDisplayName = "Success", AutoCreateRefTerm = "DoorsToConnect", AdvancedDisplay = "DoorsToConnect,bFailIfNotConnected", BlueprintInternalUseOnly = true))
	bool AddRoomToDungeon(URoom* const& Room, const TArray<int>& DoorsToConnect, bool bFailIfNotConnected = true);
	bool AddRoomToDungeon(URoom* const& Room);

private:
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
	// If ticked, the rooms location and rotation will be relative to this actor transform.
	// If unticked, the rooms will be placed relatively to the world's origin.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	bool bUseGeneratorTransform;

	// How to handle the seed at each generation call.
	// Random: Generate and use a random seed.
	// Auto Increment: Use Seed for first generation, and increment it by SeedIncrement in each subsequent generation.
	// Fixed: Use only Seed for each generation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Seed")
	ESeedType SeedType;

	// The increment number for each subsequent dungeon generation when SeedType is AutoIncrement.
	UPROPERTY(EditAnywhere, Category = "Procedural Generation|Seed", meta = (EditCondition = "SeedType==ESeedType::AutoIncrement", EditConditionHides, DisplayAfter = "Seed"))
	uint32 SeedIncrement;

	// If ticked, when trying to place a new room during a dungeon generation,
	// a box overlap test will be made to make sure the room will not spawn
	// inside existing meshes in the persistent world.
	// This is a heavy work and should be ticked only when necessary.
	// Does not have impact during gameplay. Only during the generation process.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation", AdvancedDisplay)
	bool bUseWorldCollisionChecks {false};

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (CompactNodeTitle = "Seed"))
	int32 GetSeed();

	uint32 GetUniqueId() const { return UniqueId; }
	uint64 GetGeneration() const { return Generation; }

	inline bool UseGeneratorTransform() const { return bUseGeneratorTransform; }
	FVector GetDungeonOffset() const;
	FQuat GetDungeonRotation() const;
	const FTransform& GetDungeonTransform() const;

	FORCEINLINE const UDungeonGraph* GetRooms() const { return Graph; }
	FORCEINLINE EGenerationState GetCurrentState() const { return CurrentState; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Dungeon Generator", meta = (DisplayName = "Rooms"))
	UDungeonGraph* Graph;

private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Procedural Generation|Seed", meta = (EditCondition = "SeedType!=ESeedType::Random", EditConditionHides))
	uint32 Seed;

	static uint32 GeneratorCount;

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

	// Transient. Only used to detect when occlusion setting is changed.
	bool bWasOcclusionEnabled {false};

	// Transient. Only used to detect when occlusion distance is changed.
	uint32 PreviousOcclusionDistance {0};

	// Transient. Used to count unloaded/loaded/initialized rooms during generation.
	int32 CachedTmpRoomCount {0};

	// Transient. Cached collision params used when bUseWorldCollisionChecks is true
	FCollisionQueryParams WorldCollisionParams;
};
