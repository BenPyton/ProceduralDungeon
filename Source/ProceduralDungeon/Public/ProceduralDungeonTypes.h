// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.generated.h"

UENUM(BlueprintType)
enum class EGenerationState : uint8
{
	None					UMETA(DisplayName = "None"),
	Generation 				UMETA(DisplayName = "Generation"),
	Load 					UMETA(DisplayName = "Load"),
	Initialization 			UMETA(DisplayName = "Initialization"),
	Unload 					UMETA(DisplayName = "Unload"),
	NbState					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	North = 0 				UMETA(DisplayName = "North"), // rotation = 0 (world forward)
	East = 255 				UMETA(DisplayName = "East"),  // rotation = -90 (world right)
	West = 1 				UMETA(DisplayName = "West"),  // rotation = 90 (world left)
	South = 2 				UMETA(DisplayName = "South"), // rotation = 180 (world backward)
	NbDirection = 4 		UMETA(Hidden)
};

USTRUCT()
struct FDoorDef {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FIntVector Position;
	UPROPERTY(EditAnywhere)
	EDirection Direction;
};
