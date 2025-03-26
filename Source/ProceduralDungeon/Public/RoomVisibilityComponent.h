/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticRoomVisibilityComponent.h"
#include "RoomVisitor.h"
#include "RoomVisibilityComponent.generated.h"

// Actor component to manage the visibility of an actor in the dungeon.
// Use this one if the actor is able to move room.
// If the actor remains in the same room, use UStaticRoomVisibilityComponent instead.
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent, DisplayName = "Room Visibility"))
class PROCEDURALDUNGEON_API URoomVisibilityComponent : public UStaticRoomVisibilityComponent, public IRoomVisitor
{
	GENERATED_BODY()

public:
	URoomVisibilityComponent();

	//~ BEGIN IRoomVisitor
	virtual void OnRoomEnter_Implementation(ARoomLevel* RoomLevel) override;
	virtual void OnRoomExit_Implementation(ARoomLevel* RoomLevel) override;
	//~ END IRoomVisitor
};
