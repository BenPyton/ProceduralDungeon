// Fill out your copyright notice in the Description page of Project Settings.

#include "Room.h"
#include "Door.h"
#include "Engine/World.h"
#include "Engine.h"
#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonSettings.h"
#include "../Public/Room.h"

void URoom::Init(TSubclassOf<URoomData> _RoomClass, URoom* _Parent)
{
	RoomClass = _RoomClass;
	Instance = nullptr;
	Parent = _Parent;
	Position = FIntVector(0,0,0);
	Direction = EDirection::North;

	Values = _RoomClass.GetDefaultObject();
	if (nullptr != Values)
	{
		for (int i = 0; i < Values->GetNbDoor(); i++)
		{
			Connections.Add(nullptr);
		}
	}
}

bool URoom::IsConnected(int _index)
{
	check(_index >= 0 && _index < Connections.Num());
	return Connections[_index] != nullptr;
}

void URoom::SetConnection(int Index, URoom* Room)
{
	check(Index >= 0 && Index < Connections.Num());
	Connections[Index] = Room;
}

TWeakObjectPtr<URoom> URoom::GetConnection(int Index)
{
	check(Index >= 0 && Index < Connections.Num());
	return Connections[Index];
}

// return -1 if _room = null or room is not connected
int URoom::GetConnectionIndex(URoom& Room)
{
	int Index = -1;
	if (!Connections.Find(&Room, Index))
	{
		return -1;
	}
	return Index;
}

void URoom::Instantiate(UWorld* world)
{
	if (Instance == nullptr)
	{
		Instance = UProceduralLevelStreaming::Load(world, Values, URoom::Unit() * FVector(Position), FRotator(0, -90 * (int)Direction, 0));
		UE_LOG(LogProceduralDungeon, Log, TEXT("Load room Instance: %s"), nullptr != Instance ? *Instance->GetWorldAssetPackageName() : TEXT("Null"));
	}
	else
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("Can't instantiate an already instanciated room"));
	}
}

void URoom::Destroy(UWorld * world)
{
	if (Instance != nullptr)
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("Unload room Instance: %s"), nullptr != Instance ? *Instance->GetWorldAssetPackageName() : TEXT("Null"));

		ARoomLevel* script = GetLevelScript();
		if (script != nullptr)
		{
			script->Room = nullptr;
			script->Destroy();
		}

		UProceduralLevelStreaming::Unload(world, Instance);
	}
}


ARoomLevel* URoom::GetLevelScript()
{
	if (Instance == nullptr || !IsValid(Instance))
	{
		return nullptr;
	}
	return Cast<ARoomLevel>(Instance->GetLevelScriptActor());
}

EDirection URoom::GetDoorWorldOrientation(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	return Add(Values->Doors[DoorIndex].Direction, Direction);
}

FIntVector URoom::GetDoorWorldPosition(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	return RoomToWorld(Values->Doors[DoorIndex].Position);
}

FIntVector URoom::WorldToRoom(FIntVector WorldPos)
{
	return Rotate(WorldPos - Position, Sub(EDirection::North, Direction));
}

FIntVector URoom::RoomToWorld(FIntVector RoomPos)
{
	return Rotate(RoomPos, Direction) + Position;
}

void URoom::SetRotationFromDoor(int DoorIndex, EDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Direction = Add(Sub(WorldRot, Values->Doors[DoorIndex].Direction), EDirection::South);
}

void URoom::SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Position = WorldPos - RoomToWorld(Values->Doors[DoorIndex].Position);
}

void URoom::SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Direction = Add(Sub(WorldRot, Values->Doors[DoorIndex].Direction), EDirection::South);
	Position = WorldPos - RoomToWorld(Values->Doors[DoorIndex].Position);
}


bool URoom::IsOccupied(FIntVector Cell)
{
	return Cell.X >= Position.X && Cell.X < Position.X + Values->Size.X
		&& Cell.Y >= Position.Y && Cell.Y < Position.Y + Values->Size.Y
		&& Cell.Z >= Position.Z && Cell.Z < Position.Z + Values->Size.Z;
}

void URoom::ConnectTo(int ThisDoorIndex, URoom & OtherRoom, int OtherDoorIndex)
{
	// Connect room between them
	OtherRoom.SetConnection(OtherDoorIndex, this);
	this->SetConnection(ThisDoorIndex, &OtherRoom);

	// Set position and rotation of new room in world
	EDirection OtherDoorOrientation = OtherRoom.GetDoorWorldOrientation(OtherDoorIndex);
	this->SetRotationFromDoor(ThisDoorIndex, OtherDoorOrientation);
	this->SetPositionFromDoor(ThisDoorIndex, OtherRoom.GetDoorWorldPosition(OtherDoorIndex) + URoom::GetDirection(OtherDoorOrientation));
}

FIntVector Max(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z));
}

FIntVector Min(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z));
}

// AABB Overlapping
bool URoom::Overlap(URoom& A, URoom& B)
{
	FIntVector A_firstPoint = A.Position;
	FIntVector B_firstPoint = B.Position;
	FIntVector A_secondPoint = A.RoomToWorld(A.Values->Size - FIntVector(1,1,1));
	FIntVector B_secondPoint = B.RoomToWorld(B.Values->Size - FIntVector(1,1,1));

	FIntVector A_min = Min(A_firstPoint, A_secondPoint);
	FIntVector A_max = Max(A_firstPoint, A_secondPoint);
	FIntVector B_min = Min(B_firstPoint, B_secondPoint);
	FIntVector B_max = Max(B_firstPoint, B_secondPoint);

	if (A_min.X > B_max.X) return false;
	if (A_max.X < B_min.X) return false;
	if (A_min.Y > B_max.Y) return false;
	if (A_max.Y < B_min.Y) return false;
	if (A_min.Z > B_max.Z) return false;
	if (A_max.Z < B_min.Z) return false;
	return true;
}

bool URoom::Overlap(URoom & Room, TArray<URoom*>& RoomList)
{
	bool overlap = false;
	for (int i = 0; i < RoomList.Num() && !overlap; i++)
	{
		if (Overlap(Room, *RoomList[i]))
		{
			overlap = true;
		}
	}
	return overlap;
}

EDirection URoom::Add(EDirection A, EDirection B)
{
	int8 D = (int8)A + (int8)B;
	while (D > 2) D -= 4;
	while (D <= -2) D += 4;
	return (EDirection)D;
}

EDirection URoom::Sub(EDirection A, EDirection B)
{
	int8 D = (int8)A - (int8)B;
	while (D > 2) D -= 4;
	while (D <= -2) D += 4;
	return (EDirection)D;
}

FIntVector URoom::GetDirection(EDirection O)
{
	FIntVector Dir = FIntVector::ZeroValue;
	switch (O)
	{
	case EDirection::North:
		Dir.X = 1;
		break;
	case EDirection::East:
		Dir.Y = 1;
		break;
	case EDirection::West:
		Dir.Y = -1;
		break;
	case EDirection::South:
		Dir.X = -1;
		break;
	}
	return Dir;
}

FIntVector URoom::Rotate(FIntVector Pos, EDirection Rot)
{
	FIntVector NewPos = Pos;
	switch (Rot)
	{
	case EDirection::North:
		NewPos = Pos;
		break;
	case EDirection::West:
		NewPos.Y = -Pos.X;
		NewPos.X = Pos.Y;
		break;
	case EDirection::East:
		NewPos.Y = Pos.X;
		NewPos.X = -Pos.Y;
		break;
	case EDirection::South:
		NewPos.Y = -Pos.Y;
		NewPos.X = -Pos.X;
		break;
	}
	return NewPos;
}

FVector URoom::GetRealDoorPosition(FIntVector DoorCell, EDirection DoorRot)
{
	return URoom::Unit() * (FVector(DoorCell) + 0.5f * FVector(URoom::GetDirection(DoorRot)) + FVector(0, 0, URoom::DoorOffset()));
}

FVector URoom::Unit()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->RoomUnit;
}

FVector URoom::DoorSize()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->DoorSize;
}

float URoom::DoorOffset()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->DoorOffset;
}

bool URoom::OcclusionCulling()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->OcclusionCulling;
}

bool URoom::DrawDebug()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->DrawDebug;
}
