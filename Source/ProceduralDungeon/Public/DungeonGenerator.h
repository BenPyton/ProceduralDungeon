// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "ProceduralDungeonTypes.h"
#include "DungeonGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenerationEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomEvent, TSubclassOf<URoomData>, NewRoom);

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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Generate();

	// ===== Methods that should be overriden in blueprint =====

	// Return the RoomData you want as root of the dungeon generation
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Choose First Room"))
	TSubclassOf<URoomData> ChooseFirstRoomData();

	// Return the RoomData that will be connected to the Current Room
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Choose Next Room"))
	TSubclassOf<URoomData> ChooseNextRoomData(TSubclassOf<URoomData> CurrentRoom);

	// Return the door which will be spawned between Current Room and Next Room
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Choose Door"))
	TSubclassOf<ADoor> ChooseDoor(TSubclassOf<URoomData> CurrentRoom, TSubclassOf<URoomData> NextRoom);

	// Condition to validate a dungeon Generation
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Is Valid Dungeon"))
	bool IsValidDungeon();

	// Condition to continue or stop adding room to the dungeon
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Continue To Add Room"))
	bool ContinueToAddRoom();

	// ===== Optional events =====

	// Called after unloading previous dungeon but before generating next dungeon.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Pre Generation"))
	void OnPreGeneration_BP();

	// Called after all rooms are loaded and initialized
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Post Generation"))
	void OnPostGeneration_BP();

	// Called before generating a new dungeon and each time IsValidDungeon return false
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Generation Init"))
	void OnGenerationInit_BP();

	// Called when the room NewRoom is added in the generation (but not spawned yet)
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Room Added"))
	void OnRoomAdded_BP(TSubclassOf<URoomData> NewRoom);

	// ===== Utility functions you can use in blueprint =====

	// Return true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAlreadyRoomData(TSubclassOf<URoomData> RoomData);

	// Return true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAlreadyOneRoomDataFrom(TArray<TSubclassOf<URoomData>> RoomDataList);

	// Return the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int CountRoomData(TSubclassOf<URoomData> RoomData);

	// Return the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int CountTotalRoomData(TArray<TSubclassOf<URoomData>> RoomDataList);

	// Return a random RoomData from the array provided
	UFUNCTION(BlueprintCallable)
	TSubclassOf<URoomData> GetRandomRoomData(TArray<TSubclassOf<URoomData>> RoomDataArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle="Nb Room"))
	int GetNbRoom() { return RoomList.Num(); }

	URoom* GetRoomAt(FIntVector _roomCell);

	// ===== Events =====

	UPROPERTY(BlueprintAssignable)
	FGenerationEvent OnPreGenerationEvent;

	UPROPERTY(BlueprintAssignable)
	FGenerationEvent OnPostGenerationEvent;

	UPROPERTY(BlueprintAssignable)
	FGenerationEvent OnGenerationInitEvent;

	UPROPERTY(BlueprintAssignable)
	FRoomEvent OnRoomAddedEvent;

protected:

	// ===== Implementation of blueprint native events  =====

	UFUNCTION()
	virtual TSubclassOf<URoomData> ChooseFirstRoomData_Implementation();

	UFUNCTION()
	virtual TSubclassOf<URoomData> ChooseNextRoomData_Implementation(TSubclassOf<URoomData> CurrentRoom);

	UFUNCTION()
	virtual TSubclassOf<ADoor> ChooseDoor_Implementation(TSubclassOf<URoomData> CurrentRoom, TSubclassOf<URoomData> NextRoom);

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
	virtual void OnRoomAdded(TSubclassOf<URoomData> NewRoom) {}

private:
	// Launch the generation process of the dungeon
	UFUNCTION(NetMulticast, Reliable)
	void BeginGeneration(uint32 _Seed);

	// Create virtually the dungeon (no load nor initialization of rooms)
	UFUNCTION()
	void CreateDungeon();

	// That add a room function to generate all rooms
	TArray<URoom*> AddNewRooms(URoom& _ParentRoom);

	// Instantiate a room in the scene
	void InstantiateRoom(URoom* _Room);

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
	void OnStateBegin(EGenerationState _State);
	UFUNCTION()
	void OnStateTick(EGenerationState _State);
	UFUNCTION()
	void OnStateEnd(EGenerationState _State);

	// ===== Dispatch optional events =====

	UFUNCTION()
	void DispatchPreGeneration();

	UFUNCTION()
	void DispatchPostGeneration();

	UFUNCTION()
	void DispatchGenerationInit();

	UFUNCTION()
	void DispatchRoomAdded(TSubclassOf<URoomData> NewRoom);

private:
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	EGenerationType GenerationType;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	ESeedType SeedType;
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

	EGenerationState PreviousState = EGenerationState::None;
	EGenerationState State = EGenerationState::None;
};
