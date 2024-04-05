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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoomVisitor.h"
#include "RoomObserverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomObserverEvent, ARoomLevel*, RoomLevel, AActor*, Actor);

// Room Observer that auto-(un)bind itself when it enters/exits a dungeon room.
// Could observe (be bound) multiple rooms at once if the actor overlaps multiple room.
// This component **does** track its own Room, thus the actor can move between rooms (use StaticRoomObserverComponent instead if this behavior is not needed).
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API URoomObserverComponent : public UActorComponent, public IRoomVisitor
{
	GENERATED_BODY()

public:	
	URoomObserverComponent();

	FRoomObserverEvent& OnActorEnterRoomEvent() { return ActorEnterRoomEvent; }
	FRoomObserverEvent& OnActorExitRoomEvent() { return ActorExitRoomEvent; }

protected:
	UPROPERTY(BlueprintAssignable, Category = "Room Observer", meta = (DisplayName = "On Actor Enter Room"))
	FRoomObserverEvent ActorEnterRoomEvent;

	UPROPERTY(BlueprintAssignable, Category = "Room Observer", meta = (DisplayName = "On Actor Exit Room"))
	FRoomObserverEvent ActorExitRoomEvent;

private:
	//~ BEGIN IRoomVisitor
	virtual void OnRoomEnter_Implementation(ARoomLevel* RoomLevel) override;
	virtual void OnRoomExit_Implementation(ARoomLevel* RoomLevel) override;
	//~ END IRoomVisitor

	void BindToLevel(ARoomLevel* RoomLevel, bool Bind);

	UFUNCTION()
	void OnActorEnterRoom(ARoomLevel* RoomLevel, AActor* Actor);

	UFUNCTION()
	void OnActorExitRoom(ARoomLevel* RoomLevel, AActor* Actor);

private:
	TSet<ARoomLevel*> BoundLevels;
};
