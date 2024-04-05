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

#include "Components/RoomObserverComponent.h"
#include "ProceduralDungeonLog.h"
#include "RoomLevel.h"

URoomObserverComponent::URoomObserverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomObserverComponent::OnRoomEnter_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_InfoSilent("Observer Component '%s' Enters Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	BindToLevel(RoomLevel, true);
}

void URoomObserverComponent::OnRoomExit_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_InfoSilent("Observer Component '%s' Exits Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
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
