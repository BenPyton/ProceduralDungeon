/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
