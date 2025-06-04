// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoomContainer.generated.h"

class URoom;
class URoomConnection;

UINTERFACE(MinimalAPI, NotBlueprintable, NotBlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class URoomContainer : public UInterface
{
	GENERATED_BODY()
};

/**
 * Common interface for all containers that holds rooms and their connections.
 * Currently used to get back references in URoom and URoomConnection when loaded from a saved dungeon.
 */
class PROCEDURALDUNGEON_API IRoomContainer
{
	GENERATED_BODY()

public:
	virtual URoom* GetRoomByIndex(int64 Index) const = 0;
	virtual URoomConnection* GetConnectionByIndex(int32 Index) const = 0;
};
