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

UStaticRoomObserverComponent::UStaticRoomObserverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStaticRoomObserverComponent::BeginPlay()
{
	Super::BeginPlay();
	RegisterToLevel(true);
}

void UStaticRoomObserverComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	RegisterToLevel(false);
}

void UStaticRoomObserverComponent::OnActorEnterRoom_Implementation(ARoomLevel* RoomLevel, AActor* Actor)
{
	IRoomObserver::OnActorEnterRoom_Implementation(RoomLevel, Actor);
	// Just forward the call to the delegate.
	ActorEnterRoomEvent.Broadcast(RoomLevel, Actor);
}

void UStaticRoomObserverComponent::OnActorExitRoom_Implementation(ARoomLevel* RoomLevel, AActor* Actor)
{
	IRoomObserver::OnActorExitRoom_Implementation(RoomLevel, Actor);
	// Just forward the call to the delegate.
	ActorExitRoomEvent.Broadcast(RoomLevel, Actor);
}

void UStaticRoomObserverComponent::RegisterToLevel(bool Register)
{
	ULevel* Level = GetComponentLevel();
	check(IsValid(Level));
	ARoomLevel* RoomLevel = Cast<ARoomLevel>(Level->GetLevelScriptActor());
	if (!IsValid(RoomLevel))
		return;
	RoomLevel->RegisterObserver(this, Register);
}
