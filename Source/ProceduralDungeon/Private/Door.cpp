/*
 * Copyright (c) 2019-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Door.h"
#include "Room.h"
#include "RoomLevel.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "DungeonGenerator.h"
#include "DoorType.h"
#include "ProceduralDungeonUtils.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonLog.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true; // prevent the doors from despawning on clients when server's player is too far
	NetDormancy = ENetDormancy::DORM_DormantAll;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = DefaultSceneComponent;
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(ADoor, bShouldBeLocked, Params);
	DOREPLIFETIME_WITH_PARAMS(ADoor, bShouldBeOpen, Params);
	DOREPLIFETIME_WITH_PARAMS(ADoor, RoomA, Params);
	DOREPLIFETIME_WITH_PARAMS(ADoor, RoomB, Params);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		SetActorHiddenInGame(Dungeon::OcclusionCulling()
			&& !bAlwaysVisible
			&& !(bRoomAVisible || bRoomBVisible)
		);
	}

	// Update door's lock state
	// A door is locked when ALL those conditions are met:
	// - The door is not `Always Unlocked`.
	// - The user tells the door should be locked.
	// - The door is spawned by the dungeon generator AND one of the connected rooms is locked or missing.
	const bool bPrevLocked = bLocked;
	const bool bRoomALocked = !IsValid(RoomA) || RoomA->IsLocked();
	const bool bRoomBLocked = !IsValid(RoomB) || RoomB->IsLocked();
	bLocked = !bAlwaysUnlocked && (bShouldBeLocked || (bSpawnedByDungeon && (bRoomALocked || bRoomBLocked)));

	if (bLocked != bPrevLocked)
	{
		DungeonLog_Debug("Door %s locked: %d", *GetNameSafe(this), bLocked);
		if (bLocked)
		{
			OnDoorLock();
			OnDoorLock_BP();
		}
		else
		{
			OnDoorUnlock();
			OnDoorUnlock_BP();
		}
	}

	// Update door's open state
	const bool bPrevIsOpen = bIsOpen;
	bIsOpen = bShouldBeOpen && !bLocked;
	if (bIsOpen != bPrevIsOpen)
	{
		DungeonLog_Debug("Door %s open: %d", *GetNameSafe(this), bIsOpen);
		if (bIsOpen)
		{
			OnDoorOpen();
			OnDoorOpen_BP();
		}
		else
		{
			OnDoorClose();
			OnDoorClose_BP();
		}
	}

#if ENABLE_DRAW_DEBUG
	// TODO: Place it in an editor module of the plugin
	if (Dungeon::DrawDebug() && GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		FDoorDef DoorDef;
		DoorDef.Direction = EDoorDirection::NbDirection;
		DoorDef.Type = Type;
		FDoorDef::DrawDebug(GetWorld(), DoorDef);
	}
#endif // ENABLE_DRAW_DEBUG
}

void ADoor::SetConnectingRooms(URoom* _RoomA, URoom* _RoomB)
{
	check(HasAuthority());
	SET_ACTOR_REPLICATED_PROPERTY_VALUE(RoomA, _RoomA);
	SET_ACTOR_REPLICATED_PROPERTY_VALUE(RoomB, _RoomB);
}

void ADoor::Open(bool bOpen)
{
	if (!HasAuthority())
		return;

	SET_ACTOR_REPLICATED_PROPERTY_VALUE(bShouldBeOpen, bOpen);
}

void ADoor::Lock(bool bLock)
{
	if (!HasAuthority())
		return;

	SET_ACTOR_REPLICATED_PROPERTY_VALUE(bShouldBeLocked, bLock)
}
