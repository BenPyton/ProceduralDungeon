// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralDungeonSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class PROCEDURALDUNGEON_API UProceduralDungeonSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	FVector RoomUnit;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	FVector DoorSize;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	float DoorOffset;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool OcclusionCulling;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool DrawDebug;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool OnScreenPrintDebug;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	float PrintDebugDuration;
};
