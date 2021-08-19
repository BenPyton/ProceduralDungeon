// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralLevelStreaming.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Room.generated.h"

class ARoomLevel;
class URoomData;
class ADoor;

USTRUCT()
struct FRoomConnection
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<URoom> OtherRoom = nullptr;
	int OtherDoorIndex = -1;
	ADoor* DoorInstance = nullptr;
};

UCLASS()
class PROCEDURALDUNGEON_API URoom : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TArray<FRoomConnection> Connections;

public:
	UPROPERTY()
	UProceduralLevelStreaming* Instance;
	UPROPERTY()
	FIntVector Position;
	EDoorDirection Direction;

	UPROPERTY()
	URoomData* Values = nullptr;

	TSubclassOf<URoomData> GetRoomDataClass() { return RoomClass; }

private:
	TSubclassOf<URoomData> RoomClass;

public:
	void Init(TSubclassOf<URoomData> Values);

	bool IsConnected(int Index);
	void SetConnection(int Index, URoom* Room, int OtherDoorIndex);
	TWeakObjectPtr<URoom> GetConnection(int Index);
	int GetFirstEmptyConnection();

	void Instantiate(UWorld* World);
	void Destroy(UWorld* World);
	ARoomLevel* GetLevelScript();
	bool IsInstanceLoaded();
	bool IsInstanceUnloaded();

	EDoorDirection GetDoorWorldOrientation(int DoorIndex);
	FIntVector GetDoorWorldPosition(int DoorIndex);
	int GetConnectionCount() { return Connections.Num(); }
	int GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsDoorInstanced(int _DoorIndex);
	void SetDoorInstance(int _DoorIndex, ADoor* _Door);
	int GetOtherDoorIndex(int _DoorIndex);
	void TryConnectToExistingDoors(TArray<URoom*>& RoomList);

	FIntVector WorldToRoom(FIntVector WorldPos);
	FIntVector RoomToWorld(FIntVector RoomPos);
	EDoorDirection WorldToRoom(EDoorDirection WorldRot);
	EDoorDirection RoomToWorld(EDoorDirection RoomRot);
	void SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot);
	void SetPositionFromDoor(int DoorIndex, FIntVector WorldPos);
	void SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsOccupied(FIntVector Cell);

	// AABB Overlapping
	static bool Overlap(URoom& A, URoom& B);
	static bool Overlap(URoom& Room, TArray<URoom*>& RoomList);
	static EDoorDirection Add(EDoorDirection A, EDoorDirection B);
	static EDoorDirection Sub(EDoorDirection A, EDoorDirection B);
	static EDoorDirection Opposite(EDoorDirection O);
	static FIntVector GetDirection(EDoorDirection O);
	static FIntVector Rotate(FIntVector Pos, EDoorDirection Rot);

	static FVector GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot);

	static void Connect(URoom& _RoomA, int _DoorA, URoom& _RoomB, int _DoorB);
	static URoom* GetRoomAt(FIntVector _RoomCell, TArray<URoom*>& _RoomList);

	// Plugin Settings
	static FVector Unit();
	static FVector DoorSize();
	static float DoorOffset();
	static bool OcclusionCulling();
	static bool DrawDebug();
	static bool CanLoop();
};
