/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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
#include "ProceduralDungeonTypes.h"
#include "DungeonGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenerationEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomEvent, URoomData*, NewRoom);

class ADoor;
class URoom;

UCLASS()
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
	URoomData* ChooseNextRoomData(URoomData* CurrentRoom);

	// Return the door which will be spawned between Current Room and Next Room
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Door"))
	TSubclassOf<ADoor> ChooseDoor(URoomData* CurrentRoom, URoomData* NextRoom);

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

	// Called when the room NewRoom is added in the generation (but not spawned yet)
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon Generator", meta = (DisplayName = "On Room Added"))
	void OnRoomAdded_BP(URoomData* NewRoom);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (CompactNodeTitle="Nb Room"))
	int GetNbRoom() { return RoomList.Num(); }

	URoom* GetRoomAt(FIntVector RoomCell);

	// ===== Events =====

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPreGenerationEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnPostGenerationEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FGenerationEvent OnGenerationInitEvent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeon Generator")
	FRoomEvent OnRoomAddedEvent;

protected:

	// ===== Implementation of blueprint native events  =====

	UFUNCTION()
	virtual URoomData* ChooseFirstRoomData_Implementation();

	UFUNCTION()
	virtual URoomData* ChooseNextRoomData_Implementation(URoomData* CurrentRoom);

	UFUNCTION()
	virtual TSubclassOf<ADoor> ChooseDoor_Implementation(URoomData* CurrentRoom, URoomData* NextRoom);

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
	virtual void OnRoomAdded(URoomData* NewRoom) {}

private:
	// Launch the generation process of the dungeon
	UFUNCTION(NetMulticast, Reliable, Category = "Dungeon Generator")
	void BeginGeneration(uint32 GenerationSeed);

	// Create virtually the dungeon (no load nor initialization of rooms)
	UFUNCTION()
	void CreateDungeon();

	// That add a room function to generate all rooms
	TArray<URoom*> AddNewRooms(URoom& ParentRoom);

	// Instantiate a room in the scene
	void InstantiateRoom(URoom* Room);

	// Load all room levels
	UFUNCTION()
	void LoadAllRooms();

	// unload all room levels
	UFUNCTION()
	void UnloadAllRooms();

	// ===== FSM =====

	UFUNCTION()
	void SetState(EGenerationState NewState);
	UFUNCTION()
	void OnStateBegin(EGenerationState State);
	UFUNCTION()
	void OnStateTick(EGenerationState State);
	UFUNCTION()
	void OnStateEnd(EGenerationState State);

	// ===== Dispatch optional events =====

	UFUNCTION()
	void DispatchPreGeneration();

	UFUNCTION()
	void DispatchPostGeneration();

	UFUNCTION()
	void DispatchGenerationInit();

	UFUNCTION()
	void DispatchRoomAdded(URoomData* NewRoom);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	EGenerationType GenerationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	ESeedType SeedType;

	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta=(CompactNodeTitle = "Seed"))
	int32 GetSeed();

private:
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	uint32 Seed;

	static const int MaxTry = 500;
	static const int MaxRoomTry = 10;
	FRandomStream Random;

	UPROPERTY()
	TArray<URoom*> RoomList;
	UPROPERTY()
	TArray<class ADoor*> DoorList;

	bool IsInit = false;
	int NbInitRoom = 0;
	int NbLoadedRoom = 0;
	int NbUnloadedRoom = 0;
	EGenerationState CurrentState = EGenerationState::None;
};
