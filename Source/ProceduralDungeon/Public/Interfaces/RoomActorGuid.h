/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoomActorGuid.generated.h"

UINTERFACE(BlueprintType, Blueprintable, meta = (Tooltip = "Interface to access a custom Guid for actors saved within a dungeon."))
class URoomActorGuid : public UInterface
{
	GENERATED_BODY()
};

// Interface for all saveable actors placed in room levels
// The guid must be constant across game sessions to be able to save/load the actors.
// It can be placed on ActorComponents too, but the interface on the Actor itself will be prioritized.
// Only the first component found that implements the interface will be used. Make sure to have only one to prevent any confusions.
class PROCEDURALDUNGEON_API IRoomActorGuid
{
	GENERATED_BODY()

public:
	// Return the guid associated with this actor.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Room Actor Id")
	FGuid GetGuid() const;

	// Returns true if the actor should be included in the saved dungeon.
	// Returns false to just use a Guid without the need to include the actor in saved games.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Actor Id")
	bool ShouldSaveActor() const;

	// Return the object implementing the IRoomActorGuid interface from the provided actor.
	// It can be implemented on the Actor itself or its components.
	// If both, the actor implementation will be returned.
	static UObject* GetImplementer(AActor* Actor);
};
