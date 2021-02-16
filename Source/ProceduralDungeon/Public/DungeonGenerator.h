// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "ProceduralDungeonTypes.h"
#include "DungeonGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMapEvent);

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
	// Methods that should be overriden in blueprint

	// Return the RoomData you want as root of the dungeon generation
	UFUNCTION(BlueprintImplementableEvent)
	TSubclassOf<URoomData> ChooseFirstRoomData();

	// Return the RoomData that will be connected to the Current Room
	UFUNCTION(BlueprintImplementableEvent)
	TSubclassOf<URoomData> ChooseNextRoomData(TSubclassOf<URoomData> CurrentRoom);

	// Return the door which will be spawned between Current Room and Next Room
	UFUNCTION(BlueprintImplementableEvent)
	TSubclassOf<ADoor> ChooseDoor(TSubclassOf<URoomData> CurrentRoom, TSubclassOf<URoomData> NextRoom);

	// Callback when the room NewRoom is added in the generation (but not spawned)
	UFUNCTION(BlueprintImplementableEvent)
	void OnRoomAdded(TSubclassOf<URoomData> NewRoom);

	// Called after unloading previous dungeon but before generating next dungeon.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Pre Generation"))
	void OnPreGeneration_BP();

	// Called after all rooms are loaded and initialized
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Post Generation"))
	void OnPostGeneration_BP();

	// Called before generating a new dungeon and each time IsValidDungeon return false
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Generation Init"))
	void OnGenerationInit_BP();

	// Condition to validate a dungeon Generation
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Is Valid Dungeon"))
	bool IsValidDungeon_BP();

	// Condition to continue or stop adding room to the dungeon
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Continue Adding Room"))
	bool ContinueGeneration_BP();

	// Utility methods you can use in blueprint

	// Return if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintCallable)
	bool HasAlreadyRoomData(TSubclassOf<URoomData> RoomData);

	// Return a random RoomData from the array provided
	UFUNCTION(BlueprintCallable)
	TSubclassOf<URoomData> GetRandomRoomData(TArray<TSubclassOf<URoomData>> RoomDataArray);

	UFUNCTION(BlueprintCallable)
	int GetNbRoom() { return RoomList.Num(); }


	// Change the Seed in GameState in server side
	// Do nothing when called on clients
	UFUNCTION(BlueprintCallable)
	void Generate();

	UPROPERTY(BlueprintAssignable)
	FMapEvent OnPreGenerationEvent;
	UPROPERTY(BlueprintAssignable)
	FMapEvent OnPostGenerationEvent;

private:
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	bool RandomSeed;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	bool AutoIncrementSeed;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	uint32 Seed;

	static const int MaxTry = 500;
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

private:
	// Launch the generation process of the dungeon
	UFUNCTION(NetMulticast, Reliable)
	void BeginGeneration(uint32 _Seed);

	// Create virtually the dungeon (no load nor initialization of rooms)
	UFUNCTION()
	void CreateDungeon();

	// Recursive function to generate all rooms
	void AddRooms(URoom& _FromRoom, TArray<URoom*>& _RoomStack);

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
};
