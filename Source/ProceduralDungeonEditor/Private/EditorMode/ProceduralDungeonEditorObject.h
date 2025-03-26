/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "ProceduralDungeonEditorObject.generated.h"

UCLASS(MinimalApi)
class UProceduralDungeonEditorObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Procedural Dungeon Editor")
	class UDoorType* DoorType {nullptr};
};
