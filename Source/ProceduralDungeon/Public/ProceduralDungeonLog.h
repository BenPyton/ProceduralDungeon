// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "ProceduralDungeonSettings.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProceduralDungeon, Log, All);

bool ShowLogsOnScreen(float& _duration);
void LogInfo(FString message, bool showOnScreen = true);
void LogWarning(FString message, bool showOnScreen = true);
void LogError(FString message, bool showOnScreen = true);