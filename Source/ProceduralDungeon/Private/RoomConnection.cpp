/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "RoomConnection.h"
#include "Room.h"
#include "RoomData.h"
#include "Door.h"
#include "Engine/World.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonLog.h"
#include "Engine/Engine.h"
#include "Interfaces/RoomContainer.h"
#include "Utils/DungeonSaveUtils.h"

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


bool URoomConnection::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
{
	check(!SaveData.IsValid());
	SaveData = MakeUnique<FSaveData>();

	if (!bIsLoading)
	{
		TSoftObjectPtr<URoom> RoomAWeak(RoomA.Get());
		TSoftObjectPtr<URoom> RoomBWeak(RoomB.Get());
		SaveData->RoomAID = RoomA.IsValid() ? RoomA->GetRoomID() : -1;
		SaveData->RoomBID = RoomB.IsValid() ? RoomB->GetRoomID() : -1;

		// Serializing the door instance's properties only during the save here.
		// The properties will be loaded back when the door is spawned.
		SerializeUObject(SaveData->DoorSavedData, DoorInstance.Get(), false);
	}

	Record.EnterField(AR_FIELD_NAME("RoomA")) << SaveData->RoomAID;
	Record.EnterField(AR_FIELD_NAME("RoomB")) << SaveData->RoomBID;

	Record.EnterField(AR_FIELD_NAME("DoorClass")) << DoorClass;
	Record.EnterField(AR_FIELD_NAME("DoorProperties")) << SaveData->DoorSavedData;

	if (bIsLoading)
	{
		DungeonLog_Debug("[%s] Loaded DoorClass: %s", *GetNameSafe(this), *GetNameSafe(DoorClass));
	}

	if (!bIsLoading)
	{
		// No need to keep the saved data after saving.
		SaveData.Reset();
	}

	return true;
}

bool URoomConnection::FixupReferences(UObject* Context)
{
	check(SaveData.IsValid());

	auto* RoomContainer = Cast<IRoomContainer>(Context);
	if (RoomContainer == nullptr)
	{
		DungeonLog_Error("[%s] Failed to fixup RoomConnection references: Context is not a RoomContainer.", *GetNameSafe(this));
		return false;
	}

	RoomA = RoomContainer->GetRoomByIndex(SaveData->RoomAID);
	RoomB = RoomContainer->GetRoomByIndex(SaveData->RoomBID);

	DungeonLog_Debug("[%s] Fixed up RoomConnection references: RoomA=%s, RoomB=%s", *GetNameSafe(this), *GetNameSafe(RoomA.Get()), *GetNameSafe(RoomB.Get()));

	return true;
}

void URoomConnection::PreSaveDungeon_Implementation()
{
	if (DoorInstance.IsValid() && DoorInstance->Implements<UDungeonSaveInterface>())
	{
		IDungeonSaveInterface::Execute_PreSaveDungeon(DoorInstance.Get());
	}
}

void URoomConnection::PostLoadDungeon_Implementation()
{
	SaveData.Reset();

	if (DoorInstance.IsValid() && DoorInstance->Implements<UDungeonSaveInterface>())
	{
		IDungeonSaveInterface::Execute_PostLoadDungeon(DoorInstance.Get());
	}
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

	FDoorDef DoorDef = Room->GetDoorDef(DoorId);
	FVector InstanceDoorPos = FDoorDef::GetRealDoorPosition(DoorDef);
	FQuat InstanceDoorRot = FRotator(0, 90 * static_cast<uint8>(DoorDef.Direction) + bFinalFlipped * 180, 0).Quaternion();

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

	if (SaveData.IsValid())
	{
		// Load door data back if we have some saved data.
		SerializeUObject(SaveData->DoorSavedData, Door, true);
		DungeonLog_Info("Loaded saved data for door '%s' (open: %d, lock: %d)", *GetNameSafe(Door), Door->ShouldBeOpened(), Door->ShouldBeLocked());
	}

	return Door;
}

void URoomConnection::OnRep_ID()
{
	DungeonLog_Debug("[%s] RoomConnection '%s' ID replicated: %d", *GetAuthorityName(), *GetNameSafe(this), ID);
}

void URoomConnection::OnRep_RoomA()
{
	DungeonLog_Debug("[%s] RoomConnection '%s' RoomA replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomA.Get()));
}

void URoomConnection::OnRep_RoomB()
{
	DungeonLog_Debug("[%s] RoomConnection '%s' RoomB replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomB.Get()));
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
