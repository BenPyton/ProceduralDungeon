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

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "DungeonOctree.h"
#include "ProceduralDungeonTypes.h"
#include "CollisionQueryParams.h"
#include "UObject/ScriptInterface.h"
#include "Serialization/Archive.h"
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

UENUM(meta = (Bitflags))
enum class EGeneratorFlags
{
	None				= 0,
	Generating			= 1 << 0,
	LoadSavedDungeon	= 1 << 1,
	All					= 0b11 // add new 1 for each new flags
};
ENUM_CLASS_FLAGS(EGeneratorFlags);

// Holds the data for saving a dungeon state
USTRUCT(BlueprintType)
struct FDungeonSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "GUID")
	FGuid GeneratorId;

	UPROPERTY()
	TArray<uint8> Data {};

	friend FArchive& operator<<(FArchive& Ar, FDungeonSaveData& Data);
	friend void operator<<(FStructuredArchiveSlot Slot, FDungeonSaveData& Data);
};

// This is the main actor of the plugin. The dungeon generator is responsible to generate dungeons and replicate them over the network.
// This base class is abstract. You need to override the `CreateDungeon` function to write your own generation algorithm.
UCLASS(Abstract, NotBlueprintable, BlueprintType, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ADungeonGeneratorBase : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGeneratorBase();

protected:
	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void PostActorCreated() override;
	//~ End AActor Interface

	void SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading);

public:

	// Update the seed and call the generation on all clients
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dungeon Generator")
	void Generate();

	// Unload the current dungeon
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dungeon Generator")
	void Unload();

	UFUNCTION(BlueprintPure = false, Category = "Dungeon Generator")
	void SaveDungeon(FDungeonSaveData& SaveData);

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void LoadDungeon(const FDungeonSaveData& SaveData);

	void SerializeDungeon(FArchive& Archive);

	// ===== Methods that should be overriden in blueprint =====

	// Return the door which will be spawned between Current Room and Next Room
	// @param CurrentRoom The first of both rooms to have been generated. By default the door will face this room. [DEPRECATED] Use `CurrentRoomInstance->GetRoomData` instead.
	// @param CurrentRoomInstance The room instance of one side of the door. By default the door will face this room.
	// @param NextRoom The second of both rooms to have been generated. Set Flipped to true to make the door facing this room. [DEPRECATED] Use `NextRoomInstance->GetRoomData` instead.
	// @param NextRoomInstance The room instance of the other side of the door. Set Flipped to true to make the door facing this room.
	// @param DoorType The door type set by both room data. Use IsDoorOfType function to compare a door actor class with this.
	// @param Flipped Tells which room the door is facing between CurrentRoom (false) and NextRoom (true).
	// @return The door actor class to spawn between CurrentRoom and NextRoom.
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Door"))
	TSubclassOf<ADoor> ChooseDoor(const URoomData* CurrentRoom, const URoom* CurrentRoomInstance, const URoomData* NextRoom, const URoom* NextRoomInstance, const UDoorType* DoorType, bool& Flipped);

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
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "This one is buggy, use the `Random Stream` variable getter instead."))
	const FRandomStream& GetRandomStream() const { return Random; }

	// Returns the current generation progress.
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator")
	float GetProgress() const;

	// @TODO: remove this function and use Graph->GetRoomByIndex() instead.
	URoom* GetRoomByIndex(int64 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator", meta = (WorldContext = "WorldContextObject"))
	static void SaveAllDungeons(const UObject* WorldContextObject, TArray<FDungeonSaveData>& SavedData);

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator", meta = (WorldContext = "WorldContextObject"))
	static void LoadAllDungeons(const UObject* WorldContextObject, const TArray<FDungeonSaveData>& SavedData);

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
	// Choose the door classes for all room connections.
	// This must happen *after* Graph->InitRooms() to be able to choose door class for unconnected doors.
	void ChooseDoorClasses();

	// Update the rooms visibility based on the player position
	void UpdateRoomVisibility();

	// Reset all data from a specific generation
	void Reset();

	// Clear and refill octree from the dungeon graph
	void UpdateOctree();

	// Initialize the seed depending on the seed type setting
	void UpdateSeed();

	bool IsGenerating() const { return EnumHasAllFlags(Flags, EGeneratorFlags::Generating); }
	bool IsLoadingSavedDungeon() const { return EnumHasAllFlags(Flags, EGeneratorFlags::LoadSavedDungeon); }

	void DrawDebug() const;

	// ===== FSM =====

	void SetState(EGenerationState NewState);
	void OnStateBegin(EGenerationState State);
	void OnStateTick(EGenerationState State);
	void OnStateEnd(EGenerationState State);

public:
	// If ticked, the rooms location and rotation will be relative to this actor transform.
	// If unticked, the rooms will be placed relatively to the world's origin.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Procedural Generation")
	bool bUseGeneratorTransform;

	// How to handle the seed at each generation call.
	// Random: Generate and use a random seed.
	// Auto Increment: Use Seed for first generation, and increment it by SeedIncrement in each subsequent generation.
	// Fixed: Use only Seed for each generation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Procedural Generation|Seed")
	ESeedType SeedType;

	// The increment number for each subsequent dungeon generation when SeedType is AutoIncrement.
	UPROPERTY(EditAnywhere, SaveGame, Category = "Procedural Generation|Seed", meta = (EditCondition = "SeedType==ESeedType::AutoIncrement", EditConditionHides, DisplayAfter = "Seed"))
	uint32 SeedIncrement;

	// If ticked, when trying to place a new room during a dungeon generation,
	// a box overlap test will be made to make sure the room will not spawn
	// inside existing meshes in the persistent world.
	// This is a heavy work and should be ticked only when necessary.
	// Does not have impact during gameplay. Only during the generation process.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Procedural Generation", AdvancedDisplay)
	bool bUseWorldCollisionChecks {false};

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (CompactNodeTitle = "Seed"))
	int32 GetSeed() const;

	FGuid GetGuid() const { return Id; }

	inline bool UseGeneratorTransform() const { return bUseGeneratorTransform; }
	FVector GetDungeonOffset() const;
	FQuat GetDungeonRotation() const;
	const FTransform& GetDungeonTransform() const;

	FORCEINLINE const UDungeonGraph* GetRooms() const { return Graph; }
	FORCEINLINE EGenerationState GetCurrentState() const { return CurrentState; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Dungeon Generator", meta = (DisplayName = "Rooms"))
	UDungeonGraph* Graph;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, NonPIEDuplicateTransient, TextExportTransient, Category = "GUID")
	FGuid Id;

private:
	UPROPERTY(Replicated, EditAnywhere, SaveGame, Category = "Procedural Generation|Seed", meta = (EditCondition = "SeedType!=ESeedType::Random", EditConditionHides))
	uint32 Seed;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon Generator", meta = (DisplayName = "Random Stream", AllowPrivateAccess = true))
	FRandomStream Random;

#if WITH_EDITORONLY_DATA
	// If true the dungeon will be saved in a human readable json format.
	// *WARNING*: This is only available in editor and dev builds and will not change anything in packaged builds. It should be used for debugging purposes only.
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Procedural Generation", meta = (AllowPrivateAccess = true))
	bool bUseJsonSave {false};

	// Draws the computed dungeon bounding box.
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Procedural Generation", meta = (AllowPrivateAccess = true))
	bool bDrawDebugDungeonBounds {false};
#endif

	// If true, the generator will manage the default UE navmesh system to rebuild it at the end of a generation.
	// If false, the generator will do nothing with the navigation system.
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Procedural Generation", meta = (AllowPrivateAccess = true))
	bool bRebuildNavmesh {true};

	EGenerationState CurrentState {EGenerationState::Idle};
	EGeneratorFlags Flags {EGeneratorFlags::None};

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
