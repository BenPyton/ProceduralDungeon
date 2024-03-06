/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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

#include "Door.h"
#include "Room.h"
#include "RoomLevel.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "DungeonGenerator.h"
#include "DoorType.h"
#include "ProceduralDungeonUtils.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true; // prevent the doors from despawning on clients when server's player is too far
	NetDormancy = ENetDormancy::DORM_DormantAll;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bShouldBeLocked);
	DOREPLIFETIME(ADoor, bShouldBeOpen);
	DOREPLIFETIME(ADoor, RoomA);
	DOREPLIFETIME(ADoor, RoomB);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update door visibility
	// TODO: this should not work with multiplayer games, because bHidden is replicated!
	// It works only because it is updated each frame on clients too!
	// The behavior will change if update in an event instead!
	// So, I should find another way to hide the actor... (avoiding if possible RootComponent::SetVisible)
	SetActorHiddenInGame(Dungeon::OcclusionCulling()
		&& (!bAlwaysVisible
		&& (!IsValid(RoomA) || (!RoomA->IsVisible()))
		&& (!IsValid(RoomB) || (!RoomB->IsVisible()))));

	// Update door's lock state
	const bool bPrevLocked = bLocked;
	bLocked = !bAlwaysUnlocked && (bShouldBeLocked ||
			  ((!IsValid(RoomA) || RoomA->IsLocked())
			|| (!IsValid(RoomB) || RoomB->IsLocked())));

	if (bLocked != bPrevLocked)
	{
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
		FDoorDef::DrawDebug(GetWorld(), FColor::Blue, UDoorType::GetSize(Type));
#endif // ENABLE_DRAW_DEBUG
}

void ADoor::SetConnectingRooms(URoom* _RoomA, URoom* _RoomB)
{
	check(HasAuthority());
	FlushNetDormancy();
	RoomA = _RoomA;
	RoomB = _RoomB;
}

void ADoor::Open(bool open)
{
	if (!HasAuthority())
		return;
	FlushNetDormancy();
	bShouldBeOpen = open;
}

void ADoor::Lock(bool lock)
{
	if (!HasAuthority())
		return;
	FlushNetDormancy();
	bShouldBeLocked = lock;
}
