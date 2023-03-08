/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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

#include "ProceduralDungeonLog.h"
#include "ProceduralDungeonSettings.h"

DEFINE_LOG_CATEGORY(LogProceduralDungeon);

bool ShowLogsOnScreen(float& _duration)
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	_duration = Settings->PrintDebugDuration;
	return Settings->OnScreenPrintDebug;
}

void LogInfo(FString message, bool showOnScreen)
{
	UE_LOG(LogProceduralDungeon, Log, TEXT("%s"), *message);
	float duration;
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::White, message);
	}
}

void LogWarning(FString message, bool showOnScreen)
{
	UE_LOG(LogProceduralDungeon, Warning, TEXT("%s"), *message);
	float duration;
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Yellow, message);
	}
}

void LogError(FString message, bool showOnScreen)
{
	UE_LOG(LogProceduralDungeon, Error, TEXT("%s"), *message);
	float duration;
	ShowLogsOnScreen(duration);
	GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Red, message);
}