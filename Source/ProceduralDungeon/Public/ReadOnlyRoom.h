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

#include "ProceduralDungeonTypes.h"
#include "ReadOnlyRoom.generated.h"

class URoomData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint, Documentable, Tooltip = "Allow access to only some members of Room instances during the generation process."))
class UReadOnlyRoom : public UInterface
{
	GENERATED_BODY()
};

// Interface to access some room instance's data during the generation process.
class PROCEDURALDUNGEON_API IReadOnlyRoom
{
	GENERATED_BODY()

public:
	// Returns the room data asset of this room instance.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual const URoomData* GetRoomData() const { return nullptr; }

	// Returns the unique ID (per-dungeon) of the room.
	// The first room has ID 0 and then it increases in the order of placed room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual int64 GetRoomID() const { return -1ll; }

	// Returns the position of the room (in Room Units).
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FIntVector GetPosition() const { return FIntVector::ZeroValue; }

	// Returns the direction of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual EDoorDirection GetDirection() const { return EDoorDirection::North; }

	// Returns true if all the doors of this room are connected to other rooms.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual bool AreAllDoorsConnected() const { return false; }

	// Returns true if all the doors of this room are connected to other rooms.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual int CountConnectedDoors() const { return -1; }

	// Returns the world center position of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FVector GetBoundsCenter() const { return FVector::ZeroVector; }

	// Returns the world extents (half size) of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FVector GetBoundsExtent() const { return FVector::ZeroVector; }
};
