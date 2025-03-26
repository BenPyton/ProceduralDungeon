/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "DoorTypeFactory.h"
#include "DoorType.h"
#include "AssetTypeCategories.h"
#include "ProceduralDungeonEditor.h"

UDoorTypeFactory::UDoorTypeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDoorType::StaticClass();
}

uint32 UDoorTypeFactory::GetMenuCategories() const
{
	FProceduralDungeonEditorModule& EditorModule = FModuleManager::LoadModuleChecked<FProceduralDungeonEditorModule>("ProceduralDungeonEditor");
	return EditorModule.GetAssetTypeCategory();
}

UObject* UDoorTypeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDoorType* DoorType = NewObject<UDoorType>(InParent, InClass, InName, Flags);
	return DoorType;
}
