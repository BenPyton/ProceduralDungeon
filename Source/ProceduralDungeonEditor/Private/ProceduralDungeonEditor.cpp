/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonEditor.h"
#include "ProceduralDungeonEdLog.h"
#include "Customizations/DoorDefCustomization.h"
#include "Customizations/Margin3DCustomization.h"
#include "ProceduralDungeonTypes.h"
#include "IAssetTools.h"
#include "Interfaces/IPluginManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_RoomData.h"
#include "AssetTypeActions/AssetTypeActions_DoorType.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "ProceduralDungeonEditorSettings.h"
#include "EditorMode/ProceduralDungeonEdMode.h"
#include "EditorMode/ProceduralDungeonEditorCommands.h"

#define LOCTEXT_NAMESPACE "FProceduralDungeonEditorModule"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

void FProceduralDungeonEditorModule::StartupModule()
{
	RegisterSettings();

	FProceduralDungeonEditorCommands::Register();

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
		PropertyModule.RegisterCustomPropertyTypeLayout(FMargin3D::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMargin3DCustomization::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	// Register slate style set
	{
		StyleSet = MakeShareable(new FSlateStyleSet("ProceduralDungeonStyle"));
		FString ContentDir = IPluginManager::Get().FindPlugin("ProceduralDungeon")->GetBaseDir();
		StyleSet->SetContentRoot(ContentDir);

		const FVector2D Icon20x20(20.0f, 20.0f);
		const FVector2D Icon40x40(40.0f, 40.0f);

		StyleSet->Set("ProceduralDungeon.Icon", new IMAGE_BRUSH("Resources/IconEditorMode", Icon40x40));
		StyleSet->Set("ProceduralDungeon.Icon.Small", new IMAGE_BRUSH("Resources/IconEditorMode", Icon20x20));
		StyleSet->Set("ProceduralDungeonEditor.SizeTool", new IMAGE_BRUSH("Resources/IconSizeTool", Icon20x20));
		StyleSet->Set("ProceduralDungeonEditor.SizeTool.Small", new IMAGE_BRUSH("Resources/IconSizeTool", Icon20x20));
		StyleSet->Set("ProceduralDungeonEditor.DoorTool", new IMAGE_BRUSH("Resources/IconDoorTool", Icon20x20));
		StyleSet->Set("ProceduralDungeonEditor.DoorTool.Small", new IMAGE_BRUSH("Resources/IconDoorTool", Icon20x20));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}

	FSlateIcon ModeIcon = FSlateIcon("ProceduralDungeonStyle", "ProceduralDungeon.Icon", "ProceduralDungeon.Icon.Small");
	FEditorModeRegistry::Get().RegisterMode<FProceduralDungeonEdMode>(FProceduralDungeonEdMode::EM_ProceduralDungeon, LOCTEXT("ProceduralDungeonEdModeName", "Dungeon Room"), ModeIcon, true);
}

void FProceduralDungeonEditorModule::ShutdownModule()
{
	FProceduralDungeonEditorCommands::Unregister();

	// Unregister detail customizations
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FDoorDef::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FMargin3D::StaticStruct()->GetFName());
	}

	// Unregister editor mode
	FEditorModeRegistry::Get().UnregisterMode(FProceduralDungeonEdMode::EM_ProceduralDungeon);

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
