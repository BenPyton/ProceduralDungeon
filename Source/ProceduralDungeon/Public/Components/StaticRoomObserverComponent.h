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
#include "StaticRoomObserverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStaticRoomObserverEvent, ARoomLevel*, RoomLevel, AActor*, Actor);

// Room Observer that auto-(un)bind itself at BeginPlay and EndPlay.
// This component will bind to the level it belongs to. So it needs to be placed directly in the Room map.
// This component does **not** track its own Room, thus the actor should not move between rooms (use RoomObserverComponent instead).
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API UStaticRoomObserverComponent : public UActorComponent
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
	void BindToLevel(bool Bind);

	UFUNCTION()
	void OnActorEnterRoom(ARoomLevel* RoomLevel, AActor* Visitor);

	UFUNCTION()
	void OnActorExitRoom(ARoomLevel* RoomLevel, AActor* Visitor);

private:
	bool bBound {false};
};
