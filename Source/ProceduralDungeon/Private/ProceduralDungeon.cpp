// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "ProceduralDungeon.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "ProceduralDungeonSettings.h"
#include "ProceduralDungeonLog.h"
#include "Misc/EngineVersionComparison.h"
#include "UObject/CoreRedirects.h"

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
#if ACTOR_REPLACEMENT_FIX_HACK
	ObjectReplacedHandle = FCoreUObjectDelegates::OnObjectsReinstanced.AddStatic(ObjectReplaced);
	DungeonLog_Debug("Use Actor Replacement Hack");
#endif

	TArray<FCoreRedirect> Redirects;
	Redirects.Emplace(ECoreRedirectFlags::Type_Property, TEXT("/Script/ProceduralDungeon.Room.RoomData"), TEXT("/Script/ProceduralDungeon.Room.SoftRoomData"));
	FCoreRedirects::AddRedirectList(Redirects, TEXT("ProceduralDungeon"));
}

void FProceduralDungeonModule::ShutdownModule()
{
#if ACTOR_REPLACEMENT_FIX_HACK
	FCoreUObjectDelegates::OnObjectsReinstanced.Remove(ObjectReplacedHandle);
#endif
}

IMPLEMENT_MODULE(FProceduralDungeonModule, ProceduralDungeon)
