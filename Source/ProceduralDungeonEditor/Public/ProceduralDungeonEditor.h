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
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"

class FSlateStyleSet;

class FProceduralDungeonEditorModule : public IModuleInterface
{
public:
	// ~BEGIN IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~END IModuleInterface

	FORCEINLINE EAssetTypeCategories::Type GetAssetTypeCategory() const { return AssetTypeCategory; }

private:
	void RegisterSettings();
	void UnregisterSettings();
	bool HandleSettingsSaved();

private:
	EAssetTypeCategories::Type AssetTypeCategory {EAssetTypeCategories::Type::None};
	TSharedPtr<FSlateStyleSet> StyleSet;
};
