// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FProceduralDungeonEditorCommands : public TCommands<FProceduralDungeonEditorCommands>
{
public:
	FProceduralDungeonEditorCommands();

	virtual void RegisterCommands() override;

public:
	static FName ProceduralDungeonContext;

	// Tools
	TSharedPtr<FUICommandInfo> SizeTool = nullptr;
	TSharedPtr<FUICommandInfo> DoorTool = nullptr;

	// Map
	TMap<FName, TSharedPtr<FUICommandInfo>> NameToCommandMap;
};
