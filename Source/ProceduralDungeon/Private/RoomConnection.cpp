/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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

#include "RoomConnection.h"
#include "Room.h"
#include "RoomData.h"
#include "Door.h"
#include "Engine/World.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonLog.h"
#include "Engine/Engine.h"

void URoomConnection::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	// InitialOnly is not called on newly created subobjects after the InitialCond of actor owner has already been called!!!
	//Params.Condition = COND_InitialOnly;
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, ID, Params);
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, RoomA, Params);
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, RoomADoorId, Params);
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, RoomB, Params);
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, RoomBDoorId, Params);
	DOREPLIFETIME_WITH_PARAMS(URoomConnection, DoorInstance, Params);
}

int32 URoomConnection::GetID() const
{
	return ID;
}

const TWeakObjectPtr<URoom> URoomConnection::GetRoomA() const
{
	return RoomA;
}

const TWeakObjectPtr<URoom> URoomConnection::GetRoomB() const
{
	return RoomB;
}

int32 URoomConnection::GetRoomADoorId() const
{
	return RoomADoorId;
}

int32 URoomConnection::GetRoomBDoorId() const
{
	return RoomBDoorId;
}

TWeakObjectPtr<URoom> URoomConnection::GetOtherRoom(const URoom* FromRoom) const
{
	check(FromRoom == RoomA || FromRoom == RoomB);
	return (FromRoom == RoomA) ? RoomB : RoomA;
}

int32 URoomConnection::GetOtherDoorId(const URoom* FromRoom) const
{
	check(FromRoom == RoomA || FromRoom == RoomB);
	return (FromRoom == RoomA) ? RoomBDoorId : RoomADoorId;
}

bool URoomConnection::IsDoorInstanced() const
{
	return DoorInstance.IsValid();
}

ADoor* URoomConnection::GetDoorInstance() const
{
	return DoorInstance.Get();
}

void URoomConnection::SetDoorClass(TSubclassOf<ADoor> InDoorClass, bool bInFlipped)
{
	DoorClass = InDoorClass;
	bFlipped = bInFlipped;
}

ADoor* URoomConnection::InstantiateDoor(UWorld* World, AActor* Owner, bool bUseOwnerTransform)
{
	if (!IsValid(World))
	{
		DungeonLog_Error("Can't spanw door: Invalid World.");
		return nullptr;
	}

	if (DoorInstance.IsValid())
	{
		DungeonLog_WarningSilent("Door already instanced.");
		return DoorInstance.Get();
	}

	if (nullptr == DoorClass)
		return nullptr;

	// Get next room
	const URoom* Room = RoomA.Get();
	int32 DoorId = RoomADoorId;
	bool bFinalFlipped = bFlipped;
	if (!IsValid(Room))
	{
		Room = RoomB.Get();
		DoorId = RoomBDoorId;
		bFinalFlipped = !bFinalFlipped; // Flipped is inverted when using RoomB instead of RoomA
	}

	FIntVector DoorCell = Room->GetDoorWorldPosition(DoorId);
	EDoorDirection DoorRot = Room->GetDoorWorldOrientation(DoorId);

	FVector InstanceDoorPos = FDoorDef::GetRealDoorPosition(DoorCell, DoorRot);
	FQuat InstanceDoorRot = FRotator(0, 90 * (int8)DoorRot + bFinalFlipped * 180, 0).Quaternion();

	if (bUseOwnerTransform && IsValid(Owner))
	{
		InstanceDoorPos = Owner->GetActorTransform().TransformPositionNoScale(InstanceDoorPos);
		InstanceDoorRot = Owner->GetActorTransform().TransformRotation(InstanceDoorRot);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot.Rotator(), SpawnParams);

	if (!IsValid(Door))
	{
		DungeonLog_Error("Failed to spawn Door, make sure you set door actor to always spawning.");
		return nullptr;
	}

	Door->SetConnectingRooms(RoomA.Get(), RoomB.Get());
	DoorInstance = Door;

	return Door;
}

void URoomConnection::OnRep_ID()
{
	DungeonLog_InfoSilent("[%s] RoomConnection '%s' ID replicated: %d", *GetAuthorityName(), *GetNameSafe(this), ID);
}

void URoomConnection::OnRep_RoomA()
{
	DungeonLog_InfoSilent("[%s] RoomConnection '%s' RoomA replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomA.Get()));
}

void URoomConnection::OnRep_RoomB()
{
	DungeonLog_InfoSilent("[%s] RoomConnection '%s' RoomB replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomB.Get()));
}

URoom* URoomConnection::GetOtherRoom(const URoomConnection* Conn, const URoom* FromRoom)
{
	return (Conn != nullptr) ? Conn->GetOtherRoom(FromRoom).Get() : nullptr;
}

int32 URoomConnection::GetOtherDoorId(const URoomConnection* Conn, const URoom* FromRoom)
{
	return (Conn != nullptr) ? Conn->GetOtherDoorId(FromRoom) : -1;
}

ADoor* URoomConnection::GetDoorInstance(const URoomConnection* Conn)
{
	return (Conn != nullptr) ? Conn->DoorInstance.Get() : nullptr;
}

UDoorType* URoomConnection::GetDoorType(const URoomConnection* Conn)
{
	if (!IsValid(Conn))
	{
		return nullptr;
	}

	URoom* Room = Conn->RoomA.Get();
	int32 DoorId = Conn->RoomADoorId;
	if (!IsValid(Room))
	{
		Room = Conn->RoomB.Get();
		DoorId = Conn->RoomBDoorId;
	}

	if (!IsValid(Room))
	{
		return nullptr;
	}

	return Room->GetRoomData()->Doors[DoorId].Type;
}

URoomConnection* URoomConnection::CreateConnection(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB, UObject* Outer, int32 IdInOuter)
{
	// At least one room and its door index must be valid.
	const bool bIsAValid = IsValid(RoomA) && RoomA->IsDoorIndexValid(DoorA);
	const bool bIsBValid = IsValid(RoomB) && RoomB->IsDoorIndexValid(DoorB);
	check(bIsAValid || bIsBValid);

	URoomConnection* NewConnection = NewObject<URoomConnection>(Outer);
	check(NewConnection != nullptr);

	NewConnection->ID = IdInOuter;
	NewConnection->RoomA = RoomA;
	NewConnection->RoomADoorId = DoorA;
	NewConnection->RoomB = RoomB;
	NewConnection->RoomBDoorId = DoorB;

	if (bIsAValid)
	{
		RoomA->SetConnection(DoorA, NewConnection);
	}

	if (bIsBValid)
	{
		RoomB->SetConnection(DoorB, NewConnection);
	}

	return NewConnection;
}
