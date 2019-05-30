// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ProceduralDungeon.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "ProceduralDungeonSettings.h"

#define LOCTEXT_NAMESPACE "FProceduralDungeonModule"

DEFINE_LOG_CATEGORY(LogProceduralDungeon);

void FProceduralDungeonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterSettings();
}

void FProceduralDungeonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

void FProceduralDungeonModule::RegisterSettings()
{
	// Registering some settings is just a matter of exposing the default UObject of
	// your desired class, feel free to add here all those settings you want to expose
	// to your LDs or artists.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Procedural Dungeon",
			LOCTEXT("RuntimeGeneralSettingsName", "Procedural Dungeon"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Configuration for the Procedural Dungeon plugin"),
			GetMutableDefault<UProceduralDungeonSettings>()
		);

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FProceduralDungeonModule::HandleSettingsSaved);
		}
	}
}

void FProceduralDungeonModule::UnregisterSettings()
{
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Procedural Dungeon");
	}
}

// Callback for when the settings were saved.
bool FProceduralDungeonModule::HandleSettingsSaved()
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	bool ResaveSettings = false;

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralDungeonModule, ProceduralDungeon)