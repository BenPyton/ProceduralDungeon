// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoomVisitor.generated.h"

class ARoomLevel;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (Tooltip = "Interface which adds events to an actor (or a component) when the actor enters/exits a dungeon room."))
class URoomVisitor : public UInterface
{
	GENERATED_BODY()
};

// Interface which adds events to an actor (or a component) when the actor enters/exits a dungeon room.
class PROCEDURALDUNGEON_API IRoomVisitor
{
	GENERATED_BODY()

public:
	// This function is called when the actor enters a dungeon room.
	// @param RoomLevel The room level blueprint this actor has entered. You can use the Room accessor from here.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Visitor")
	void OnRoomEnter(ARoomLevel* RoomLevel);

	// This function is called when the actor exits a dungeon room.
	// @param RoomLevel The room level blueprint this actor has exited. You can use the Room accessor from here.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Visitor")
	void OnRoomExit(ARoomLevel* RoomLevel);
};
