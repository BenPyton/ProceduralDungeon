/*
 * Copyright (c) 2019-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonSettings.h"
#include "HAL/IConsoleManager.h"

UProceduralDungeonSettings::UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Dungeon settings
	RoomUnit = FVector(1000, 1000, 400);
	DoorSize = FVector(40, 640, 400);
	DoorOffset = 0.0f;
	CanLoop = true;
	MaxGenerationTry = 500;
	MaxRoomPlacementTry = 10;
	RoomLimit = 100;

	// Occlusion settings
	OcclusionCulling = true;
	//LegacyOcclusion = true;
	OcclusionDistance = 2;
	OccludeDynamicActors = true;

	// Debug settings
	DrawDebug = true;
	bDrawOnlyWhenEditingRooms = false;
	ShowRoomOrigin = false;
	bFlipDoorArrowSide = false;
	DoorArrowLength = 300.0f;
	DoorArrowHeadSize = 300.0f;
	OnScreenPrintDebug = false;
	PrintDebugDuration = 60.0f;

	// Register console variables.

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Occlusion")
		, OcclusionCulling
		, TEXT("Enable/disable the plugin's occlusion culling system.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Occlusion.Distance")
		, OcclusionDistance
		, TEXT("Change the number of room shown by the plugin's occlusion culling system.\n")
		  TEXT("1 means only the player's room is visible. 0 or negative means no room visible at all. 2 or more will show the connected rooms to the player based on their number of connection.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Occlusion.DynamicActors")
		, OccludeDynamicActors
		, TEXT("Enable/disable the occlusion of actors with a RoomVisibility component attached on them.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Debug.Draw")
		, DrawDebug
		, TEXT("Enable/disable the debug drawings of the rooms and doors.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Debug.Draw.EditingOnly")
		, bDrawOnlyWhenEditingRooms
		, TEXT("Enable/disable the debug drawings to be only shown when editing a room level.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Debug.Log.OnScreen")
		, OnScreenPrintDebug
		, TEXT("Enable/disable the on-screen logging of the plugin.")
		, EConsoleVariableFlags::ECVF_Cheat
	);

	IConsoleManager::Get().RegisterConsoleVariableRef(TEXT("pd.Debug.Log.Duration")
		, PrintDebugDuration
		, TEXT("Change the on-screen logging duration (in seconds) of the plugin.")
		, EConsoleVariableFlags::ECVF_Cheat
	);
}
