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
#include "RoomObserver.h"
#include "RoomVisitor.h"
#include "RoomObserverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomObserverEvent, ARoomLevel*, RoomLevel, AActor*, Actor);

// Room Observer that auto-(un)register itself at BeginPlay, EndPlay and when it moves from one room to another.
// Could observe (be registered) in multiple rooms at once if the actor overlaps multiple room.
// This component **does** track its own Room, thus the actor can move between rooms (use StaticRoomObserverComponent instead if this behavior is not needed).
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API URoomObserverComponent : public UActorComponent, public IRoomObserver, public IRoomVisitor
{
	GENERATED_BODY()

public:	
	URoomObserverComponent();

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

	//~ BEGIN IRoomObserver
	virtual void OnActorEnterRoom_Implementation(ARoomLevel* RoomLevel, AActor* Actor) override;
	virtual void OnActorExitRoom_Implementation(ARoomLevel* RoomLevel, AActor* Actor) override;
	//~ END IRoomObserver
};
