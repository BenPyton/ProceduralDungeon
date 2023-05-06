/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ProceduralDungeonEditor.h"
#include "ProceduralDungeonEdLog.h"
#include "DoorDefCustomization.h"
#include "ProceduralDungeonTypes.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_RoomData.h"
#include "AssetTypeActions/AssetTypeActions_DoorType.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "ProceduralDungeonEditorSettings.h"

#define LOCTEXT_NAMESPACE "FProceduralDungeonEditorModule"

void FProceduralDungeonEditorModule::StartupModule()
{
	RegisterSettings();

	// Register assets in the "Procedural Dungeon" category
	{
		IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTypeCategory = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("ProceduralDungeon")), LOCTEXT("ProceduralDungeonCategory", "Procedural Dungeon"));
		TSharedPtr<FAssetTypeActions_RoomData> RoomDataAction = MakeShareable(new FAssetTypeActions_RoomData());
		AssetToolsModule.RegisterAssetTypeActions(RoomDataAction.ToSharedRef());
		TSharedPtr<FAssetTypeActions_DoorType> DoorTypeAction = MakeShareable(new FAssetTypeActions_DoorType());
		AssetToolsModule.RegisterAssetTypeActions(DoorTypeAction.ToSharedRef());
	}

	// Register detail customizations
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout(FDoorDef::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDoorDefCustomization::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

void FProceduralDungeonEditorModule::ShutdownModule()
{
	// Unregister detail customizations
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FDoorDef::StaticStruct()->GetFName());
	}
	
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

void FProceduralDungeonEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Editor", "Procedural Dungeon",
			LOCTEXT("ProceduralDungeonEditorSettingsName", "Procedural Dungeon"),
			LOCTEXT("ProceduralDungeonEditorSettingsDescription", "Configuration for the Procedural Dungeon plugin"),
			GetMutableDefault<UProceduralDungeonEditorSettings>()
		);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FProceduralDungeonEditorModule::HandleSettingsSaved);
		}
	}
}

void FProceduralDungeonEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Procedural Dungeon");
	}
}

bool FProceduralDungeonEditorModule::HandleSettingsSaved()
{
	UProceduralDungeonEditorSettings* Settings = GetMutableDefault<UProceduralDungeonEditorSettings>();
	bool ResaveSettings = false;

	// Here check and resave if any changes have been made

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralDungeonEditorModule, ProceduralDungeonEditor)