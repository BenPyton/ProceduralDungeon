/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
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
