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
#include "StaticRoomObserverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStaticRoomObserverEvent, ARoomLevel*, RoomLevel, AActor*, Actor);

// Room Observer that auto-(un)register itself at BeginPlay and EndPlay.
// This component will register to the level it belongs to. So it needs to be placed directly in the Room map.
// This component does **not** track its own Room, thus the actor should not be moved between rooms (use RoomObserverComponent instead).
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API UStaticRoomObserverComponent : public UActorComponent, public IRoomObserver
{
	GENERATED_BODY()

public:	
	UStaticRoomObserverComponent();

	FStaticRoomObserverEvent& OnActorEnterRoomEvent() { return ActorEnterRoomEvent; }
	FStaticRoomObserverEvent& OnActorExitRoomEvent() { return ActorExitRoomEvent; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

protected:
	UPROPERTY(BlueprintAssignable, Category = "Room Observer", meta = (DisplayName = "On Actor Enter Room"))
	FStaticRoomObserverEvent ActorEnterRoomEvent;

	UPROPERTY(BlueprintAssignable, Category = "Room Observer", meta = (DisplayName = "On Actor Exit Room"))
	FStaticRoomObserverEvent ActorExitRoomEvent;

private:
	//~ BEGIN IRoomObserver
	virtual void OnActorEnterRoom_Implementation(ARoomLevel* RoomLevel, AActor* Visitor) override;
	virtual void OnActorExitRoom_Implementation(ARoomLevel* RoomLevel, AActor* Visitor) override;
	//~ END IRoomObserver

	void RegisterToLevel(bool Register);
};
