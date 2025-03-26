/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Components/StaticRoomObserverComponent.h"
#include "RoomLevel.h"
#include "Engine/Level.h"

UStaticRoomObserverComponent::UStaticRoomObserverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStaticRoomObserverComponent::BeginPlay()
{
	Super::BeginPlay();
	BindToLevel(true);
}

void UStaticRoomObserverComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	BindToLevel(false);
}

void UStaticRoomObserverComponent::OnActorEnterRoom(ARoomLevel* RoomLevel, AActor* Actor)
{
	// Just forward the call to the delegate.
	ActorEnterRoomEvent.Broadcast(RoomLevel, Actor);
}

void UStaticRoomObserverComponent::OnActorExitRoom(ARoomLevel* RoomLevel, AActor* Actor)
{
	// Just forward the call to the delegate.
	ActorExitRoomEvent.Broadcast(RoomLevel, Actor);
}

void UStaticRoomObserverComponent::BindToLevel(bool Bind)
{
	if (bBound == Bind)
		return;

	ULevel* Level = GetComponentLevel();
	check(IsValid(Level));
	ARoomLevel* RoomLevel = Cast<ARoomLevel>(Level->GetLevelScriptActor());
	if (!IsValid(RoomLevel))
		return;

	if (Bind)
	{
		RoomLevel->ActorEnterRoomEvent.AddDynamic(this, &UStaticRoomObserverComponent::OnActorEnterRoom);
		RoomLevel->ActorExitRoomEvent.AddDynamic(this, &UStaticRoomObserverComponent::OnActorExitRoom);
	}
	else
	{
		RoomLevel->ActorEnterRoomEvent.RemoveDynamic(this, &UStaticRoomObserverComponent::OnActorEnterRoom);
		RoomLevel->ActorExitRoomEvent.RemoveDynamic(this, &UStaticRoomObserverComponent::OnActorExitRoom);
	}
	bBound = Bind;
}
