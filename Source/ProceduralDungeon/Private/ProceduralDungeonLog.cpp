// Fill out your copyright notice in the Description page of Project Settings.
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
	if(showOnScreen && ShowLogsOnScreen(duration))
	{
		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Red, message);
	}
}