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

#include "Room.h"
#include "Door.h"
#include "Engine/World.h"
#include "Engine.h"
#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGenerator.h"

URoom::URoom()
	: Super()
	, Instance(nullptr)
	, RoomData(nullptr)
	, GeneratorOwner(nullptr)
	, Connections()
{
}

void URoom::Init(URoomData* Data, ADungeonGenerator* Generator, int32 RoomId)
{
	RoomData = Data;
	Instance = nullptr;
	Position = FIntVector(0, 0, 0);
	Direction = EDoorDirection::North;
	GeneratorOwner = Generator;
	Id = RoomId;

	if (IsValid(RoomData))
	{
		for (int i = 0; i < RoomData->GetNbDoor(); i++)
		{
			Connections.Add(FRoomConnection());
		}
	}
	else
	{
		LogError("No RoomData provided.");
	}
}

bool URoom::IsConnected(int Index)
{
	check(Index >= 0 && Index < Connections.Num());
	return Connections[Index].OtherRoom != nullptr;
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
	for (int i = 0; i < Connections.Num(); ++i)
	{
		if (Connections[i].OtherRoom == nullptr)
		{
			return i;
		}
	}
	return -1;
}

void URoom::Instantiate(UWorld* World)
{
	if (Instance == nullptr)
	{
		if (!IsValid(RoomData))
		{
			LogError("Failed to instantiate the room: it has no RoomData.");
			return;
		}

		FVector offset(0, 0, 0);
		FQuat rotation = FQuat::Identity;
		FString nameSuffix = FString::FromInt(Id);
		if (GeneratorOwner != nullptr)
		{
			offset = GeneratorOwner->GetDungeonOffset();
			rotation = GeneratorOwner->GetDungeonRotation();
			nameSuffix = FString::FromInt(GeneratorOwner->GetUniqueId()) + TEXT("_") + nameSuffix;
		}

		FVector FinalLocation = rotation.RotateVector(URoom::Unit() * FVector(Position)) + offset;
		FQuat FinalRotation = rotation * FRotator(0, -90 * (int)Direction, 0).Quaternion();
		Instance = UProceduralLevelStreaming::Load(World, RoomData, nameSuffix, FinalLocation, FinalRotation.Rotator());
		UE_LOG(LogProceduralDungeon, Log, TEXT("Load room Instance: %s"), nullptr != Instance ? *Instance->GetWorldAssetPackageName() : TEXT("Null"));
	}
	else
	{
		LogError("Failed to instantiate the room: it is already instanciated.");
	}
}

void URoom::Destroy(UWorld* World)
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

		UProceduralLevelStreaming::Unload(World, Instance);
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
	if (Instance == nullptr || !IsValid(Instance))
	{
		return true;
	}
	return Instance->IsLevelLoaded();
}

bool URoom::IsInstanceUnloaded()
{
	if (Instance == nullptr || !IsValid(Instance))
	{
		return true;
	}
	return Instance->IsLevelUnloaded();
}

EDoorDirection URoom::GetDoorWorldOrientation(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	return Add(RoomData->Doors[DoorIndex].Direction, Direction);
}

FIntVector URoom::GetDoorWorldPosition(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	return RoomToWorld(RoomData->Doors[DoorIndex].Position);
}

int URoom::GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot)
{
	FIntVector localPos = WorldToRoom(WorldPos);
	EDoorDirection localRot = WorldToRoom(WorldRot);

	for (int i = 0; i < RoomData->Doors.Num(); ++i)
	{
		const FDoorDef door = RoomData->Doors[i];
		if (door.Position == localPos && door.Direction == localRot)
			return i;
	}
	return -1;
}

bool URoom::IsDoorInstanced(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	return IsValid(Connections[DoorIndex].DoorInstance);
}

void URoom::SetDoorInstance(int DoorIndex, ADoor* Door)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Connections[DoorIndex].DoorInstance = Door;
}

int URoom::GetOtherDoorIndex(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	return Connections[DoorIndex].OtherDoorIndex;
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
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Direction = Add(Sub(WorldRot, RoomData->Doors[DoorIndex].Direction), EDoorDirection::South);
}

void URoom::SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Position = WorldPos - RoomToWorld(RoomData->Doors[DoorIndex].Position);
}

void URoom::SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Direction = Sub(WorldRot, RoomData->Doors[DoorIndex].Direction);
	Position = WorldPos - RoomToWorld(RoomData->Doors[DoorIndex].Position);
}

bool URoom::IsOccupied(FIntVector Cell)
{
	FIntVector local = WorldToRoom(Cell);
	return local.X >= 0 && local.X < RoomData->Size.X
		&& local.Y >= 0 && local.Y < RoomData->Size.Y
		&& local.Z >= 0 && local.Z < RoomData->Size.Z;
}

void URoom::TryConnectToExistingDoors(TArray<URoom*>& RoomList)
{
	for (int i = 0; i < RoomData->GetNbDoor(); ++i)
	{
		EDoorDirection dir = GetDoorWorldOrientation(i);
		FIntVector pos = GetDoorWorldPosition(i) + URoom::GetDirection(dir);
		URoom* otherRoom = GetRoomAt(pos, RoomList);

		if (IsValid(otherRoom))
		{
			int j = otherRoom->GetDoorIndexAt(pos, URoom::Opposite(dir));
			if (j >= 0) // -1 if no door
			{
				Connect(*this, i, *otherRoom, j);
			}
		}
	}
}

FBoxCenterAndExtent URoom::GetBounds() const
{
	return RoomData->GetBounds(GetTransform());
}

FTransform URoom::GetTransform() const
{
	FTransform Transform;
	Transform.SetLocation(FVector(Position) * URoom::Unit());
	Transform.SetRotation(GetRotation(Direction));
	return Transform;
}

FIntVector Max(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z));
}

FIntVector Min(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z));
}

void URoom::SetVisible(bool Visible)
{
	if (URoom::UseLegacyOcclusion())
	{
		GetLevelScript()->SetActorsVisible(Visible);
	}
	else if(IsValid(Instance))
	{
		// TODO: make the level be veisible again, I don't know why it is not visible although
		// the Visible and Loaded of StreamingLevel are correctly set to true 
		// and the Loaded of Level instance inside it is also set to true...
		// In the meantime, only the legacy version will remains.
		//Instance->SetShouldBeVisible(Visible);
		//Instance->BroadcastLevelVisibleStatus(Instance->GetWorld(), Instance->GetWorldAssetPackageFName(), Visible);
	}
}

void URoom::SetPlayerInside(bool PlayerInside)
{
	if (bPlayerInside == PlayerInside)
		return;

	bPlayerInside = PlayerInside;
	UpdateVisibility();

	for (int i = 0; i < GetConnectionCount(); ++i)
	{
		TWeakObjectPtr<URoom> OtherRoom = GetConnection(i);
		if (OtherRoom.IsValid())
			OtherRoom->UpdateVisibility();
	}
}

// AABB Overlapping
bool URoom::Overlap(URoom& A, URoom& B)
{
	FIntVector A_firstPoint = A.Position;
	FIntVector B_firstPoint = B.Position;
	FIntVector A_secondPoint = A.RoomToWorld(A.RoomData->Size - FIntVector(1, 1, 1));
	FIntVector B_secondPoint = B.RoomToWorld(B.RoomData->Size - FIntVector(1, 1, 1));

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

bool URoom::Overlap(URoom& Room, TArray<URoom*>& RoomList)
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

FQuat URoom::GetRotation(EDoorDirection O)
{
	return FRotator(0.0f, -90.0f * (int8)O, 0.0f).Quaternion();
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

FVector URoom::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool includeOffset)
{
	return URoom::Unit() * (FVector(DoorCell) + 0.5f * FVector(URoom::GetDirection(DoorRot)) + FVector(0, 0, includeOffset ? URoom::DoorOffset() : 0));
}

void URoom::Connect(URoom& RoomA, int DoorA, URoom& RoomB, int DoorB)
{
	RoomA.SetConnection(DoorA, &RoomB, DoorB);
	RoomB.SetConnection(DoorB, &RoomA, DoorA);
}

URoom* URoom::GetRoomAt(FIntVector RoomCell, TArray<URoom*>& RoomList)
{
	for (auto it = RoomList.begin(); it != RoomList.end(); ++it)
	{
		if (IsValid(*it) && (*it)->IsOccupied(RoomCell))
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

bool URoom::UseLegacyOcclusion()
{
	//UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	//return Settings->LegacyOcclusion;
	return true;
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


void URoom::UpdateVisibility()
{
	if (!URoom::OcclusionCulling())
	{
		// TODO: Force visibility
		return;
	}

	bool PrevIsVisible = bIsVisible;
	bIsVisible = bPlayerInside;
	for (int i = 0; i < GetConnectionCount(); i++)
	{
		if (GetConnection(i).IsValid()
			&& GetConnection(i)->IsPlayerInside())
		{
			bIsVisible = true;
		}
	}

	if(PrevIsVisible != bIsVisible)
		SetVisible(bIsVisible);
}
