// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "Components/DoorComponent.h"
#include "Room.h"
#include "RoomLevel.h"
#include "RoomConnection.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "DungeonGenerator.h"
#include "DoorType.h"
#include "ProceduralDungeonUtils.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonLog.h"

UDoorComponent::UDoorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
	SetIsReplicatedByDefault(true);
}

void UDoorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(UDoorComponent, RoomConnection, Params);
}

// Called every frame
void UDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const URoom* RoomA = GetRoomA();
	const URoom* RoomB = GetRoomB();

	// Tells if the door actor has been spawned by the dungeon generator or not.
	// At least one of the room is valid when spawned by the dungeon generator.
	// Both rooms are invalid if door has been spawned by another way.
	const bool bSpawnedByDungeon = IsValid(RoomA) || IsValid(RoomB);

	// The door manages itself its own visibility only when it has been spawned by the dungeon generator.
	// If the door is placed in a RoomLevel or spawned by the user in other means, it is the responsibility
	// of the RoomLevel or the user to manage the door's visibility.
	if (bSpawnedByDungeon)
	{
		const bool bRoomAVisible = IsValid(RoomA) && RoomA->IsVisible();
		const bool bRoomBVisible = IsValid(RoomB) && RoomB->IsVisible();

		// Update door visibility
		// A door is hidden ONLY when ALL those conditions are met:
		// - The Room Culling is enabled.
		// - The door is not `Always Visible`.
		// - Both connected rooms are not visible.
		// @TODO: this should not work with multiplayer games, because bHidden is replicated!
		// It works only because it is updated each frame on clients too!
		// The behavior will change if bHidden is updated once in a wile by an event instead!
		// So, I should find another way to hide the actor... (avoiding if possible RootComponent::SetVisible)
		AActor* Owner = GetOwner();
		if (IsValid(Owner))
		{
			Owner->SetActorHiddenInGame(Dungeon::OcclusionCulling()
				&& !bAlwaysVisible
				&& !(bRoomAVisible || bRoomBVisible)
			);
		}
	}

	// Update door's lock state
	// A door is locked when ALL those conditions are met:
	// - The door is not `Always Unlocked`.
	// - The user tells the door should be locked.
	// - The door is spawned by the dungeon generator AND one of the connected rooms is locked or missing.
	const bool bPrevLocked = bLocked;
	const bool bRoomALocked = !IsValid(RoomA) || RoomA->IsLocked();
	const bool bRoomBLocked = !IsValid(RoomB) || RoomB->IsLocked();
	bLocked = !bAlwaysUnlocked && (ShouldBeLocked() || (bSpawnedByDungeon && (bRoomALocked || bRoomBLocked)));

	if (bLocked != bPrevLocked)
	{
		DungeonLog_Debug("Door %s locked: %d", *GetNameSafe(this), bLocked);
		OnDoorLock(bLocked);
		OnDoorLock_BP(bLocked);
		OnDoorLocked.Broadcast(this, bLocked);
	}

	// Update door's open state
	const bool bPrevIsOpen = bIsOpen;
	bIsOpen = ShouldBeOpen() && !bLocked;
	if (bIsOpen != bPrevIsOpen)
	{
		DungeonLog_Debug("Door %s open: %d", *GetNameSafe(this), bIsOpen);
		OnDoorOpen(bIsOpen);
		OnDoorOpen_BP(bIsOpen);
		OnDoorOpened.Broadcast(this, bIsOpen);
	}

#if ENABLE_DRAW_DEBUG
	// TODO: Place it in an editor module of the plugin
	if (Dungeon::DrawDebug() && GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		FDoorDef DoorDef;
		DoorDef.Direction = EDoorDirection::NbDirection;
		DoorDef.Type = Type;
		FDoorDef::DrawDebug(GetWorld(), DoorDef, FVector::ZeroVector);
	}
#endif // ENABLE_DRAW_DEBUG
}

void UDoorComponent::SetRoomConnection_Implementation(URoomConnection* InRoomConnection)
{
	check(OwnerHasAuthority());
	SET_COMPONENT_REPLICATED_PROPERTY_VALUE(RoomConnection, InRoomConnection);
}

void UDoorComponent::Open(bool bOpen)
{
	if (!OwnerHasAuthority())
		return;

	if (!IsValid(RoomConnection))
		return;

	RoomConnection->SetDoorOpen(bOpen);
}

void UDoorComponent::Lock(bool bLock)
{
	if (!OwnerHasAuthority())
		return;

	if (!IsValid(RoomConnection))
		return;

	RoomConnection->SetDoorLocked(bLock);
}

bool UDoorComponent::ShouldBeOpen() const
{
	if (!IsValid(RoomConnection))
		return false;

	return RoomConnection->IsDoorOpen();
}

bool UDoorComponent::ShouldBeLocked() const
{
	if (!IsValid(RoomConnection))
		return false;

	return RoomConnection->IsDoorLocked();
}

URoom* UDoorComponent::GetRoomA() const
{
	if (!IsValid(RoomConnection))
		return nullptr;

	return RoomConnection->GetRoomA().Get();
}

URoom* UDoorComponent::GetRoomB() const
{
	if (!IsValid(RoomConnection))
		return nullptr;

	return RoomConnection->GetRoomB().Get();
}

bool UDoorComponent::OwnerHasAuthority() const
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
		return false;

	return Owner->HasAuthority();
}

void UDoorComponent::OnRep_RoomConnection()
{
	DungeonLog_Debug("Door %s OnRep_RoomConnection: %s", *GetNameSafe(this), *GetNameSafe(RoomConnection));
}
