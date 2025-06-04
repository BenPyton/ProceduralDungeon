// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "AssetTypeActions_DoorType.h"
#include "DoorType.h"
#include "Modules/ModuleManager.h"
#include "ProceduralDungeonEditor.h"

#define LOCTEXT_NAMESPACE "ProceduralDungeonEditor"

FAssetTypeActions_DoorType::FAssetTypeActions_DoorType()
{
}

FText FAssetTypeActions_DoorType::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DoorType", "Door Type");
}

UClass* FAssetTypeActions_DoorType::GetSupportedClass() const
{
	return UDoorType::StaticClass();
}

FColor FAssetTypeActions_DoorType::GetTypeColor() const
{
	return FColor(255, 50, 128);
}

uint32 FAssetTypeActions_DoorType::GetCategories()
{
	return EAssetTypeCategories::None; // Defined in the Factory
}

bool FAssetTypeActions_DoorType::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

#undef LOCTEXT_NAMESPACE
