// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "AssetTypeActions_RoomData.h"
#include "RoomData.h"
#include "Modules/ModuleManager.h"
#include "ProceduralDungeonEditor.h"

#define LOCTEXT_NAMESPACE "ProceduralDungeonEditor"

FAssetTypeActions_RoomData::FAssetTypeActions_RoomData()
{
}

FText FAssetTypeActions_RoomData::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_RoomData", "Room Data");
}

UClass* FAssetTypeActions_RoomData::GetSupportedClass() const
{
	return URoomData::StaticClass();
}

FColor FAssetTypeActions_RoomData::GetTypeColor() const
{
	return FColor(255, 50, 128);
}

uint32 FAssetTypeActions_RoomData::GetCategories()
{
	return EAssetTypeCategories::None; // Defined in the Factory
}

bool FAssetTypeActions_RoomData::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

#undef LOCTEXT_NAMESPACE
