// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralDungeonSettings.h"

UProceduralDungeonSettings::UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RoomUnit = FVector(1000, 1000, 400);
	DoorSize = FVector(40, 640, 400);
	DoorOffset = 0.0f;
	OcclusionCulling = true;
	DrawDebug = true;
	OnScreenPrintDebug = false;
	PrintDebugDuration = 60.0f;
}
