// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "PDLog.h"
#include "Containers/UnrealString.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralDungeon, Log, All);

void LogOnScreen(const FString& Message, FColor Color, bool bForceOnScreen = false);

#if NO_LOGGING
#define _DungeonLog_PrivateImpl(...)
#else
// Private implementation. Dot not use it.
#define _DungeonLog_PrivateImpl(LogCategory, Verbosity, Detailed, ForceOnScreen, Color, Format, ...) \
{ \
	_PD_LOG_PrivateImpl(LogCategory, Verbosity, Detailed, Format, ##__VA_ARGS__); \
	LogOnScreen(FString::Printf(TEXT(Format), ##__VA_ARGS__), Color, ForceOnScreen); \
}
#endif // NO_LOGGING

// Logs a message to output and possibly on screen too
// No LOG_CATEGORY to define
#define DungeonLog_Debug(Format, ...) _PD_LOG_PrivateImpl(LogProceduralDungeon, VeryVerbose, true, Format, ##__VA_ARGS__)
#define DungeonLog_Info(Format, ...) _DungeonLog_PrivateImpl(LogProceduralDungeon, Log, false, false, FColor::White, Format, ##__VA_ARGS__)
#define DungeonLog_InfoSilent(Format, ...) _PD_LOG_PrivateImpl(LogProceduralDungeon, Log, false, Format, ##__VA_ARGS__)
#define DungeonLog_Warning(Format, ...) _DungeonLog_PrivateImpl(LogProceduralDungeon, Warning, true, false, FColor::Yellow, Format, ##__VA_ARGS__)
#define DungeonLog_WarningSilent(Format, ...) _PD_LOG_PrivateImpl(LogProceduralDungeon, Warning, true, Format, ##__VA_ARGS__)
#define DungeonLog_Error(Format, ...) _DungeonLog_PrivateImpl(LogProceduralDungeon, Error, true, true, FColor::Red, Format, ##__VA_ARGS__)
