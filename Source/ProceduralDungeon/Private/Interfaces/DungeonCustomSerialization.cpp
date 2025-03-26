/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Interfaces/DungeonCustomSerialization.h"
#include "ProceduralDungeonLog.h"
#include "ProceduralDungeonUtils.h"

bool IDungeonCustomSerialization::DispatchFixupReferences(UObject* Obj, UObject* Context)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [Context](UObject* O) {
		auto* Custom = Cast<IDungeonCustomSerialization>(O);
		if (nullptr != Custom)
		{
			Custom->FixupReferences(Context);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));
	return true;
}
