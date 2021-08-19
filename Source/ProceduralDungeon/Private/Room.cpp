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

#include "Room.h"
#include "Door.h"
#include "Engine/World.h"
#include "Engine.h"
#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"

void URoom::Init(TSubclassOf<URoomData> _RoomClass)
{
	RoomClass = _RoomClass;
	Instance = nullptr;
	Position = FIntVector(0,0,0);
	Direction = EDoorDirection::North;

	Values = _RoomClass.GetDefaultObject();
	if (nullptr != Values)
	{
		for (int i = 0; i < Values->GetNbDoor(); i++)
		{
			Connections.Add(FRoomConnection());
		}
	}
	else
	{
		LogError("No RoomData provided.");
	}
}

bool URoom::IsConnected(int _index)
{
	check(_index >= 0 && _index < Connections.Num());
	return Connections[_index].OtherRoom != nullptr;
}

void URoom::SetConnection(int Index, URoom* Room, int OtherIndex)
{
	check(Index >= 0 && Index < Connections.Num());
	Connections[Index].OtherRoom = Room;
	Connections[Index].OtherDoorIndex = OtherIndex;
}

TWeakObjectPtr<URoom> URoom::GetConnection(int Index)
{
	check(Index >= 0 && Index < Connections.Num());
	return Connections[Index].OtherRoom;
}

int URoom::GetFirstEmptyConnection()
{
	for(int i = 0; i < Connections.Num(); ++i)
	{
		if(Connections[i].OtherRoom == nullptr)
		{
			return i;
		}
	}
	return -1;
}

void URoom::Instantiate(UWorld* world)
{
	if (Instance == nullptr)
	{
		if(Values == nullptr)
		{
			LogError("Failed to instantiate the room: it has no RoomData.");
			return;
		}

		Instance = UProceduralLevelStreaming::Load(world, Values, URoom::Unit() * FVector(Position), FRotator(0, -90 * (int)Direction, 0));
		UE_LOG(LogProceduralDungeon, Log, TEXT("Load room Instance: %s"), nullptr != Instance ? *Instance->GetWorldAssetPackageName() : TEXT("Null"));
	}
	else
	{
		LogError("Failed to instantiate the room: it is already instanciated.");
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

bool URoom::IsInstanceLoaded()
{
	if(Instance == nullptr || !IsValid(Instance))
	{
		return true;
	}
	return Instance->IsLevelLoaded();
}

bool URoom:: IsInstanceUnloaded()
{
	if(Instance == nullptr || !IsValid(Instance))
	{
		return true;
	}
	return Instance->IsLevelUnloaded();
}

EDoorDirection URoom::GetDoorWorldOrientation(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	return Add(Values->Doors[DoorIndex].Direction, Direction);
}

FIntVector URoom::GetDoorWorldPosition(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	return RoomToWorld(Values->Doors[DoorIndex].Position);
}

int URoom::GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot)
{
	FIntVector localPos = WorldToRoom(WorldPos);
	EDoorDirection localRot = WorldToRoom(WorldRot);

	for(int i = 0; i < Values->Doors.Num(); ++i)
	{
		const FDoorDef door = Values->Doors[i];
		if(door.Position == localPos && door.Direction == localRot)
			return i;
	}
	return -1;
}

bool URoom::IsDoorInstanced(int _DoorIndex)
{
	check(_DoorIndex >= 0 && _DoorIndex < Values->Doors.Num());
	return IsValid(Connections[_DoorIndex].DoorInstance);
}

void URoom::SetDoorInstance(int _DoorIndex, ADoor* _Door)
{
	check(_DoorIndex >= 0 && _DoorIndex < Values->Doors.Num());
	Connections[_DoorIndex].DoorInstance = _Door;
}

int URoom::GetOtherDoorIndex(int _DoorIndex)
{
	check(_DoorIndex >= 0 && _DoorIndex < Values->Doors.Num());
	return Connections[_DoorIndex].OtherDoorIndex;
}

FIntVector URoom::WorldToRoom(FIntVector WorldPos)
{
	return Rotate(WorldPos - Position, Sub(EDoorDirection::North, Direction));
}

FIntVector URoom::RoomToWorld(FIntVector RoomPos)
{
	return Rotate(RoomPos, Direction) + Position;
}

EDoorDirection URoom::WorldToRoom(EDoorDirection WorldRot)
{
	return Sub(WorldRot, Direction);
}

EDoorDirection URoom::RoomToWorld(EDoorDirection RoomRot)
{
	return Add(RoomRot, Direction);
}

void URoom::SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Direction = Add(Sub(WorldRot, Values->Doors[DoorIndex].Direction), EDoorDirection::South);
}

void URoom::SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Position = WorldPos - RoomToWorld(Values->Doors[DoorIndex].Position);
}

void URoom::SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < Values->Doors.Num());
	Direction = Sub(WorldRot, Values->Doors[DoorIndex].Direction);
	Position = WorldPos - RoomToWorld(Values->Doors[DoorIndex].Position);
}


bool URoom::IsOccupied(FIntVector Cell)
{
	FIntVector local = WorldToRoom(Cell);
	return local.X >= 0 && local.X < Values->Size.X
		&& local.Y >= 0 && local.Y < Values->Size.Y
		&& local.Z >= 0 && local.Z < Values->Size.Z;
}

void URoom::TryConnectToExistingDoors(TArray<URoom*>& _RoomList)
{
	for(int i = 0; i < Values->GetNbDoor(); ++i)
	{
		EDoorDirection dir = GetDoorWorldOrientation(i);
		FIntVector pos = GetDoorWorldPosition(i) + URoom::GetDirection(dir);
		URoom* otherRoom = GetRoomAt(pos, _RoomList);

		if(IsValid(otherRoom))
		{
			int j = otherRoom->GetDoorIndexAt(pos, URoom::Opposite(dir));
			if(j >= 0) // -1 if no door
			{
				Connect(*this, i, *otherRoom, j);
			}
		}
	}
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

EDoorDirection URoom::Add(EDoorDirection A, EDoorDirection B)
{
	int8 D = (int8)A + (int8)B;
	while (D > 2) D -= 4;
	while (D <= -2) D += 4;
	return (EDoorDirection)D;
}

EDoorDirection URoom::Sub(EDoorDirection A, EDoorDirection B)
{
	int8 D = (int8)A - (int8)B;
	while (D > 2) D -= 4;
	while (D <= -2) D += 4;
	return (EDoorDirection)D;
}

EDoorDirection URoom::Opposite(EDoorDirection O)
{
	return Add(O, EDoorDirection::South);
}

FIntVector URoom::GetDirection(EDoorDirection O)
{
	FIntVector Dir = FIntVector::ZeroValue;
	switch (O)
	{
	case EDoorDirection::North:
		Dir.X = 1;
		break;
	case EDoorDirection::East:
		Dir.Y = 1;
		break;
	case EDoorDirection::West:
		Dir.Y = -1;
		break;
	case EDoorDirection::South:
		Dir.X = -1;
		break;
	}
	return Dir;
}

FIntVector URoom::Rotate(FIntVector Pos, EDoorDirection Rot)
{
	FIntVector NewPos = Pos;
	switch (Rot)
	{
	case EDoorDirection::North:
		NewPos = Pos;
		break;
	case EDoorDirection::West:
		NewPos.Y = -Pos.X;
		NewPos.X = Pos.Y;
		break;
	case EDoorDirection::East:
		NewPos.Y = Pos.X;
		NewPos.X = -Pos.Y;
		break;
	case EDoorDirection::South:
		NewPos.Y = -Pos.Y;
		NewPos.X = -Pos.X;
		break;
	}
	return NewPos;
}

FVector URoom::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot)
{
	return URoom::Unit() * (FVector(DoorCell) + 0.5f * FVector(URoom::GetDirection(DoorRot)) + FVector(0, 0, URoom::DoorOffset()));
}

void URoom::Connect(URoom& _RoomA, int _DoorA, URoom& _RoomB, int _DoorB)
{
	_RoomA.SetConnection(_DoorA, &_RoomB, _DoorB);
	_RoomB.SetConnection(_DoorB, &_RoomA, _DoorA);
}

URoom* URoom::GetRoomAt(FIntVector _RoomCell, TArray<URoom*>& _RoomList)
{
	for(auto it = _RoomList.begin(); it != _RoomList.end(); ++it)
	{
		if(IsValid(*it) && (*it)->IsOccupied(_RoomCell))
		{
			return *it;
		}
	}
	return nullptr;
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

bool URoom::CanLoop()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->CanLoop;
}

