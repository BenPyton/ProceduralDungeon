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
#include "DungeonSaveInterface.generated.h"

UINTERFACE(BlueprintType, Blueprintable, meta = (Tooltip = "Give access to some serialization events to actors saved within a dungeon."))
class UDungeonSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to add some events to the saved actors/objects during the save/load process of the dungeon.
 */
class PROCEDURALDUNGEON_API IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	// Called just before serializing this object into the dungeon save.
	// Useful to initialize some saved variables based on actor states.
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void DungeonPreSerialize(bool bIsLoading);

	// Called just after deserializing this object from the dungeon save
	// Useful to initialize some actor states based on saved variables.
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void DungeonPostSerialize(bool bIsLoading);

	// Called first before saving the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void PreSaveDungeon();

	// Called last after loading the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void PostLoadDungeon();

	static void DispatchPreSaveEvent(UObject* Obj);
	static void DispatchPostLoadEvent(UObject* Obj);
};
