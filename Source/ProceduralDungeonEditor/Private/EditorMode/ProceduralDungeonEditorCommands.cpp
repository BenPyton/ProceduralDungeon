/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonEditorCommands.h"
#include "Tools/ProceduralDungeonEditorTool_Size.h"
#include "Tools/ProceduralDungeonEditorTool_Door.h"

#define LOCTEXT_NAMESPACE "ProceduralDungeonEditorCommands"

FName FProceduralDungeonEditorCommands::ProceduralDungeonContext = TEXT("ProceduralDungeonEditor");

FProceduralDungeonEditorCommands::FProceduralDungeonEditorCommands()
	: TCommands<FProceduralDungeonEditorCommands>
	(
		FProceduralDungeonEditorCommands::ProceduralDungeonContext, // Context name for fast lookup
		NSLOCTEXT("Contexts", "ProceduralDungeonEditor", "Procedural Dungeon Editor"), // Localized context name for displaying
		NAME_None, // Parent
		FName("ProceduralDungeonStyle") // Icon Style Set
	)
{
}

void FProceduralDungeonEditorCommands::RegisterCommands()
{

	UI_COMMAND(SizeTool, "Size", "Change room size by dragging points in the viewport.", EUserInterfaceActionType::RadioButton, FInputChord());
	NameToCommandMap.Add("Tool_Size", SizeTool);

	UI_COMMAND(DoorTool, "Door", "Add doors easily by clicking in the viewport.", EUserInterfaceActionType::RadioButton, FInputChord());
	NameToCommandMap.Add("Tool_Door", DoorTool);
}

#undef LOCTEXT_NAMESPACE
