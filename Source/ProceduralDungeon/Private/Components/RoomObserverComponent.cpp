/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Components/RoomObserverComponent.h"
#include "ProceduralDungeonLog.h"
#include "RoomLevel.h"

URoomObserverComponent::URoomObserverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomObserverComponent::OnRoomEnter_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_Debug("[Observer] '%s' Enters Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	BindToLevel(RoomLevel, true);
}

void URoomObserverComponent::OnRoomExit_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_Debug("[Observer] '%s' Exits Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	BindToLevel(RoomLevel, false);
}

void URoomObserverComponent::OnActorEnterRoom(ARoomLevel* RoomLevel, AActor* Actor)
{
	// Just forward the call to the delegate.
	ActorEnterRoomEvent.Broadcast(RoomLevel, Actor);
}

void URoomObserverComponent::OnActorExitRoom(ARoomLevel* RoomLevel, AActor* Actor)
{
	// Just forward the call to the delegate.
	ActorEnterRoomEvent.Broadcast(RoomLevel, Actor);
}

void URoomObserverComponent::BindToLevel(ARoomLevel* RoomLevel, bool Bind)
{
	if (BoundLevels.Contains(RoomLevel) == Bind)
		return;

	if (!IsValid(RoomLevel))
		return;

	if (Bind)
	{
		RoomLevel->ActorEnterRoomEvent.AddDynamic(this, &URoomObserverComponent::OnActorEnterRoom);
		RoomLevel->ActorExitRoomEvent.AddDynamic(this, &URoomObserverComponent::OnActorExitRoom);
		BoundLevels.Add(RoomLevel);
	}
	else
	{
		RoomLevel->ActorEnterRoomEvent.RemoveDynamic(this, &URoomObserverComponent::OnActorEnterRoom);
		RoomLevel->ActorExitRoomEvent.RemoveDynamic(this, &URoomObserverComponent::OnActorExitRoom);
		BoundLevels.Remove(RoomLevel);
	}
}
