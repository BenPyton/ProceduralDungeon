// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "ProceduralDungeonEditorTool.h"
#include "EditorMode/ProceduralDungeonEdMode.h"
#include "RoomLevel.h"
#include "RoomData.h"

URoomData* FProceduralDungeonEditorTool::GetRoomData() const
{
	auto Level = EdMode->GetLevel();
	if (!Level.IsValid())
		return nullptr;

	return Level.Get()->Data;
}
