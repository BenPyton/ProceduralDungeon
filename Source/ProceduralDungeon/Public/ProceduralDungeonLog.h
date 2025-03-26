/*
 * Copyright (c) 2019-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "ProceduralDungeonSettings.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralDungeon, Log, All);

void LogOnScreen(const FString& Message, FColor Color, bool bForceOnScreen = false);

#if NO_LOGGING
#define _DungeonLog_PrivateImpl(...) {}
#else
// Private implementation. Dot not use it.
#define _DungeonLog_PrivateImpl(ShowOnScreen, ForceOnScreen, Detailed, Color, Verbosity, Format, ...) \
{ \
	if constexpr (Detailed) \
		{ UE_LOG(LogProceduralDungeon, Verbosity, TEXT("[%s:%d] " Format), *FString(__FUNCTION__), __LINE__, ##__VA_ARGS__); } \
	else \
		{ UE_LOG(LogProceduralDungeon, Verbosity, TEXT(Format), ##__VA_ARGS__); } \
	if constexpr (ShowOnScreen) \
		LogOnScreen(FString::Printf(TEXT(Format), ##__VA_ARGS__), Color, ForceOnScreen); \
}
#endif // NO_LOGGING

// Logs error message to output and on screen
#define DungeonLog_Debug(Format, ...) \
	_DungeonLog_PrivateImpl(false, false, true, FColor::White, VeryVerbose, Format, ##__VA_ARGS__)

// Logs info message to output and on screen
#define DungeonLog_Info(Format, ...) \
	_DungeonLog_PrivateImpl(true, false, false, FColor::White, Log, Format, ##__VA_ARGS__)

// Logs info message *only* to output
#define DungeonLog_InfoSilent(Format, ...) \
	_DungeonLog_PrivateImpl(false, false, false, FColor::White, Log, Format, ##__VA_ARGS__)

// Logs warning message to output and on screen
#define DungeonLog_Warning(Format, ...) \
	_DungeonLog_PrivateImpl(true, false, true, FColor::Yellow, Warning, Format, ##__VA_ARGS__)

// Logs warning message *only* to output
#define DungeonLog_WarningSilent(Format, ...) \
	_DungeonLog_PrivateImpl(false, false, true, FColor::Yellow, Warning, Format, ##__VA_ARGS__)

// Logs error message to output and on screen
#define DungeonLog_Error(Format, ...) \
	_DungeonLog_PrivateImpl(true, true, true, FColor::Red, Error, Format, ##__VA_ARGS__)
