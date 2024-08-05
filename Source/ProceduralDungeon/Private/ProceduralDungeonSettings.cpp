/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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
