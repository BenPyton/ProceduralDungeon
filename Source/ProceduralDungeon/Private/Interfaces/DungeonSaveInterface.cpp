/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Interfaces/DungeonSaveInterface.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"

void IDungeonSaveInterface::DispatchPreSaveEvent(UObject* Obj)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Dungeon Pre Save' events from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [](UObject* O) {
		if (IsValid(O) && O->Implements<UDungeonSaveInterface>())
		{
			IDungeonSaveInterface::Execute_PreSaveDungeon(O);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj));
}

void IDungeonSaveInterface::DispatchPostLoadEvent(UObject* Obj)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [](UObject* O) {
		if (IsValid(O) && O->Implements<UDungeonSaveInterface>())
		{
			IDungeonSaveInterface::Execute_PostLoadDungeon(O);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj));
}
