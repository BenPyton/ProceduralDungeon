/*
 * Copyright (c) 2019-2021 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeon.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"
#include "Misc/EngineVersionComparison.h"

#define LOCTEXT_NAMESPACE "FProceduralDungeonModule"

#if WITH_EDITOR && UE_VERSION_NEWER_THAN(5, 4, 0)
#define ACTOR_REPLACEMENT_FIX_HACK 1
#else
#define ACTOR_REPLACEMENT_FIX_HACK 0
#endif

// ----- Hack to fix Room references issues of RoomLevel actors in PIE for UE 5.4
#if ACTOR_REPLACEMENT_FIX_HACK
#include "RoomLevel.h"
#include "Room.h"

FDelegateHandle ObjectReplacedHandle;
void ObjectReplaced(const FCoreUObjectDelegates::FReplacementObjectMap& ReplacementMap)
{
	for (const auto& Pair : ReplacementMap)
	{
		ARoomLevel* OldActor = Cast<ARoomLevel>(Pair.Key);
		ARoomLevel* NewActor = Cast<ARoomLevel>(Pair.Value);

		if (!OldActor || !NewActor)
			continue;

		if (OldActor->HasAllFlags(EObjectFlags::RF_ClassDefaultObject) || NewActor->HasAllFlags(EObjectFlags::RF_ClassDefaultObject))
			continue;

		URoom* RoomInstance = OldActor->GetRoom();
		OldActor->Room = nullptr;
		if (!IsValid(RoomInstance))
			continue;

		// Fixup Room reference not properly carried over during actor replacement process
		NewActor->Init(RoomInstance);
		DungeonLog_Debug("Fixed Room reference ('%s' -> '%s')", *GetNameSafe(OldActor), *GetNameSafe(NewActor));
	}
}
#endif
// ----- End Hack

void FProceduralDungeonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	RegisterSettings();

#if ACTOR_REPLACEMENT_FIX_HACK
	ObjectReplacedHandle = FCoreUObjectDelegates::OnObjectsReinstanced.AddStatic(ObjectReplaced);
	DungeonLog_Debug("Use Actor Replacement Hack");
#endif
}

void FProceduralDungeonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}

#if ACTOR_REPLACEMENT_FIX_HACK
	FCoreUObjectDelegates::OnObjectsReinstanced.Remove(ObjectReplacedHandle);
#endif
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
