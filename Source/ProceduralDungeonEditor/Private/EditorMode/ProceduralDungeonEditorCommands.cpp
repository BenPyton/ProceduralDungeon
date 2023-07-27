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
