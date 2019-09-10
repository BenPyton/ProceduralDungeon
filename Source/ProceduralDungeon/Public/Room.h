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

UCLASS()
class PROCEDURALDUNGEON_API URoom : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TArray<TWeakObjectPtr<URoom>> Connections;

public:
	UPROPERTY()
	UProceduralLevelStreaming* Instance;
	UPROPERTY()
	URoom* Parent;
	FIntVector Position;
	EDirection Direction;

	UPROPERTY()
	URoomData* Values = nullptr;

	TSubclassOf<URoomData> GetRoomDataClass() { return RoomClass; }

private:
	TSubclassOf<URoomData> RoomClass;

public:
	void Init(TSubclassOf<URoomData> Values, URoom* Parent = nullptr);

	bool IsConnected(int Index);
	void SetConnection(int Index, URoom* Room);
	TWeakObjectPtr<URoom> GetConnection(int Index);
	int GetConnectionIndex(URoom& Room);

	void Instantiate(UWorld* World);
	void Destroy(UWorld* World);
	ARoomLevel* GetLevelScript();

	EDirection GetDoorWorldOrientation(int DoorIndex);
	FIntVector GetDoorWorldPosition(int DoorIndex);
	int GetConnectionCount() { return Connections.Num(); }

	FIntVector WorldToRoom(FIntVector WorldPos);
	FIntVector RoomToWorld(FIntVector RoomPos);
	void SetRotationFromDoor(int DoorIndex, EDirection WorldRot);
	void SetPositionFromDoor(int DoorIndex, FIntVector WorldPos);
	void SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDirection WorldRot);
	bool IsOccupied(FIntVector Cell);

	void ConnectTo(int ThisDoorIndex, URoom& OtherRoom, int OtherDoorIndex);


	// AABB Overlapping
	static bool Overlap(URoom& A, URoom& B);
	static bool Overlap(URoom& Room, TArray<URoom*>& RoomList);
	static EDirection Add(EDirection A, EDirection B);
	static EDirection Sub(EDirection A, EDirection B);
	static FIntVector GetDirection(EDirection O);
	static FIntVector Rotate(FIntVector Pos, EDirection Rot);

	static FVector GetRealDoorPosition(FIntVector DoorCell, EDirection DoorRot);

	// Plugin Settings
	static FVector Unit();
	static FVector DoorSize();
	static float DoorOffset();
	static bool OcclusionCulling();
	static bool DrawDebug();
};
