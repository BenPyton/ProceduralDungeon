// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralDungeonEditor, Verbose, All);

#define DungeonEd_LogInfo(Format, ...) \
	UE_LOG(LogProceduralDungeonEditor, Log, TEXT(Format), ##__VA_ARGS__)

#define DungeonEd_LogWarning(Format, ...) \
	UE_LOG(LogProceduralDungeonEditor, Warning, TEXT(Format), ##__VA_ARGS__)

#define DungeonEd_LogError(Format, ...) \
	UE_LOG(LogProceduralDungeonEditor, Error, TEXT(Format), ##__VA_ARGS__)
