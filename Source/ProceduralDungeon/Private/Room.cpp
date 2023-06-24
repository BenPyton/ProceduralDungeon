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
#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGenerator.h"

URoom::URoom()
	: Super()
	, Connections()
{
}

void URoom::Init(URoomData* Data, ADungeonGenerator* Generator, int32 RoomId)
{
	RoomData = Data;
	GeneratorOwner = Generator;
	Id = RoomId;
	Instance = nullptr;
	Position = FIntVector(0, 0, 0);
	Direction = EDoorDirection::North;

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

	SetVisible(false);
}

bool URoom::IsConnected(int Index) const
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

TWeakObjectPtr<URoom> URoom::GetConnection(int Index) const
{
	check(Index >= 0 && Index < Connections.Num());
	return Connections[Index].OtherRoom;
}

int URoom::GetFirstEmptyConnection() const
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
		FQuat FinalRotation = rotation * ToQuaternion(Direction);
		Instance = UProceduralLevelStreaming::Load(World, RoomData, nameSuffix, FinalLocation, FinalRotation.Rotator());

		if (!IsValid(Instance))
		{
			LogError("Error when instantiating the room: unknown error.");
			return;
		}

		LogInfo(FString::Printf(TEXT("Load room Instance: %s"), *Instance->GetWorldAssetPackageName()));
		Instance->OnLevelLoaded.AddDynamic(this, &URoom::OnInstanceLoaded);
	}
	else
	{
		LogError("Failed to instantiate the room: it is already instanciated.");
	}
}

void URoom::Destroy(UWorld* World)
{
	if (IsValid(Instance))
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("Unload room Instance: %s"), nullptr != Instance ? *Instance->GetWorldAssetPackageName() : TEXT("Null"));

		ARoomLevel* Script = GetLevelScript();
		if (IsValid(Script))
		{
			Script->Room = nullptr;
			Script->Destroy();
		}

		UProceduralLevelStreaming::Unload(World, Instance);
	}
}

void URoom::OnInstanceLoaded()
{
	check(IsValid(Instance));
	Instance->OnLevelLoaded.RemoveDynamic(this, &URoom::OnInstanceLoaded);

	ARoomLevel* Script = GetLevelScript();
	if (!IsValid(Script))
	{
		LogError("Error when instantiating the room: the level blueprint does not derive from RoomLevel.");
		return;
	}

	Script->Init(this);
}

ARoomLevel* URoom::GetLevelScript() const
{
	if (Instance == nullptr || !IsValid(Instance))
	{
		return nullptr;
	}
	return Cast<ARoomLevel>(Instance->GetLevelScriptActor());
}

bool URoom::IsInstanceLoaded() const
{
	return IsValid(Instance) ? Instance->IsLevelLoaded() : false;
}

bool URoom::IsInstanceUnloaded() const
{
	return IsValid(Instance) ? Instance->IsLevelUnloaded() : true;
}

bool URoom::IsInstanceInitialized() const
{
	ARoomLevel* Script = GetLevelScript();
	return (IsValid(Script)) ? Script->IsInit() : false;
}

EDoorDirection URoom::GetDoorWorldOrientation(int DoorIndex)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	return RoomData->Doors[DoorIndex].Direction + Direction;
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

FIntVector URoom::WorldToRoom(const FIntVector& WorldPos) const
{
	return Rotate(WorldPos - Position, -Direction);
}

FIntVector URoom::RoomToWorld(const FIntVector& RoomPos) const
{
	return Rotate(RoomPos, Direction) + Position;
}

EDoorDirection URoom::WorldToRoom(const EDoorDirection& WorldRot) const
{
	return WorldRot - Direction;
}

EDoorDirection URoom::RoomToWorld(const EDoorDirection& RoomRot) const
{
	return RoomRot + Direction;
}

FBoxMinAndMax URoom::WorldToRoom(const FBoxMinAndMax& WorldBox) const
{
	return Rotate(WorldBox - Position, -Direction);
}

FBoxMinAndMax URoom::RoomToWorld(const FBoxMinAndMax& RoomBox) const
{
	return Rotate(RoomBox, Direction) + Position;
}

void URoom::SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Direction = WorldRot - RoomData->Doors[DoorIndex].Direction;
}

void URoom::SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Position = WorldPos - Rotate(RoomData->Doors[DoorIndex].Position, Direction);
}

void URoom::SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot)
{
	check(DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num());
	Direction = WorldRot - RoomData->Doors[DoorIndex].Direction;
	Position = WorldPos - Rotate(RoomData->Doors[DoorIndex].Position, Direction);
}

bool URoom::IsOccupied(FIntVector Cell)
{
	FIntVector local = WorldToRoom(Cell);
	FBoxMinAndMax Bounds = RoomData->GetIntBounds();
	return local.X >= Bounds.Min.X && local.X < Bounds.Max.X
		&& local.Y >= Bounds.Min.Y && local.Y < Bounds.Max.Y
		&& local.Z >= Bounds.Min.Z && local.Z < Bounds.Max.Z;
}

void URoom::TryConnectToExistingDoors(TArray<URoom*>& RoomList)
{
	for (int i = 0; i < RoomData->GetNbDoor(); ++i)
	{
		if (IsConnected(i))
			continue;

		EDoorDirection dir = GetDoorWorldOrientation(i);
		FIntVector pos = GetDoorWorldPosition(i) + ToIntVector(dir);
		URoom* otherRoom = GetRoomAt(pos, RoomList);

		if (IsValid(otherRoom))
		{
			int j = otherRoom->GetDoorIndexAt(pos, ~dir);
			if (j >= 0 // -1 if no door
				&& FDoorDef::AreCompatible(RoomData->Doors[i], otherRoom->RoomData->Doors[j])) 
			{
				Connect(*this, i, *otherRoom, j);
			}
		}
	}
}

FBoxCenterAndExtent URoom::GetBounds() const
{
	check(IsValid(RoomData));
	return RoomData->GetBounds(GetTransform());
}

FBoxCenterAndExtent URoom::GetLocalBounds() const
{
	check(IsValid(RoomData));
	return RoomData->GetBounds();
}

FBoxMinAndMax URoom::GetIntBounds() const
{
	check(IsValid(RoomData));
	return RoomToWorld(RoomData->GetIntBounds());
}

FTransform URoom::GetTransform() const
{
	FTransform Transform;
	Transform.SetLocation(FVector(Position) * URoom::Unit());
	Transform.SetRotation(ToQuaternion(Direction));
	return Transform;
}

void URoom::SetVisible(bool Visible)
{
	if (Visible == bIsVisible)
		return;

	bIsVisible = Visible;

	if (URoom::UseLegacyOcclusion())
	{
		ARoomLevel* LevelScript = GetLevelScript();
		if(IsValid(LevelScript))
			LevelScript->SetActorsVisible(Visible);
	}
	else if(IsValid(Instance))
	{
		// TODO: make the level be visible again, I don't know why it is not visible although
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
}

// AABB Overlapping
bool URoom::Overlap(const URoom& A, const URoom& B)
{
	FBoxMinAndMax BoxA = A.GetIntBounds();
	FBoxMinAndMax BoxB = B.GetIntBounds();
	return FBoxMinAndMax::Overlap(BoxA, BoxB);
}

bool URoom::Overlap(const URoom& Room, const TArray<URoom*>& RoomList)
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

FVector URoom::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool includeOffset)
{
	const FVector CellPosition = FVector(DoorCell);
	const FVector DirectionOffset = !DoorRot ? FVector::ZeroVector : (0.5f * ToVector(DoorRot));
	const FVector HeightOffset = includeOffset ? FVector(0, 0, URoom::DoorOffset()) : FVector::ZeroVector;
	return URoom::Unit() * (CellPosition + DirectionOffset + HeightOffset);
}

void URoom::Connect(URoom& RoomA, int DoorA, URoom& RoomB, int DoorB)
{
	RoomA.SetConnection(DoorA, &RoomB, DoorB);
	RoomB.SetConnection(DoorB, &RoomA, DoorA);
}

URoom* URoom::GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList)
{
	for (URoom* Room : RoomList)
	{
		if (IsValid(Room) && Room->IsOccupied(RoomCell))
		{
			return Room;
		}
	}
	return nullptr;
}

// =================== Plugin's Settings ========================
// TODO: move them in UProceduralDungeonSettings instead?

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

uint32 URoom::OcclusionDistance()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->OcclusionDistance;
}

bool URoom::OccludeDynamicActors()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	return Settings->OcclusionCulling && Settings->OccludeDynamicActors;
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
