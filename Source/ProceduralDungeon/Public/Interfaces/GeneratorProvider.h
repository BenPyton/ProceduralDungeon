// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GeneratorProvider.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable, NotBlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UGeneratorProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for classes that give access to a ADungeonGeneratorBase instance.
 * @TODO: Currently only used to resolve URoom::GeneratorOwner references when loading a saved dungeon.
 * It would be better in a future version to decouple the URoom from the DungeonGenerator and instead
 * pass some Interface references to the functions needed (currently a Transform and a RandomStream).
 * I just want to say that this interface is just temporary and must not be used by users of the plugin,
 * as it will certainly be removed in a near future version of the plugin.
 */
class IGeneratorProvider
{
	GENERATED_BODY()

public:
	virtual class ADungeonGeneratorBase* GetGenerator() const = 0;
};
