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
#include "DungeonCustomSerialization.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable, NotBlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UDungeonCustomSerialization : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PROCEDURALDUNGEON_API IDungeonCustomSerialization
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Serialize non-trivial object properties (e.g. UObject pointers)
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) = 0;

	// Fixup object references after loading
	virtual bool FixupReferences(UObject* Context) { return true; }

	// Calls FixupReferences on Obj and its subobjects.
	static bool DispatchFixupReferences(UObject* Obj, UObject* Context);
};
