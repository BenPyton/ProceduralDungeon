/*
 * Copyright (c) 2019-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonLog.h"
#include "ProceduralDungeonSettings.h"
#include "Engine/Engine.h" // GEngine

DEFINE_LOG_CATEGORY(LogProceduralDungeon);

namespace
{
	bool ShowLogsOnScreen(float& _duration)
	{
		const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
		_duration = Settings->PrintDebugDuration;
		return Settings->OnScreenPrintDebug;
	}
}

void LogOnScreen(const FString& Message, FColor Color, bool bForceOnScreen)
{
	float Duration;
	if (::ShowLogsOnScreen(Duration) || bForceOnScreen)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}
