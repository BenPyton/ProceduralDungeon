// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.generated.h"

UENUM()
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
enum class EDoorDirection : uint8
{
	North = 0 				UMETA(DisplayName = "North"), // rotation = 0 (world forward)
	East = 255 				UMETA(DisplayName = "East"),  // rotation = -90 (world right)
	West = 1 				UMETA(DisplayName = "West"),  // rotation = 90 (world left)
	South = 2 				UMETA(DisplayName = "South"), // rotation = 180 (world backward)
	NbDirection = 4 		UMETA(Hidden)
};

UENUM()
enum class EGenerationType : uint8
{
	DFS = 0 				UMETA(DisplayName = "Depth First", Tooltip = "Make the dungeon more linear"),
	BFS = 1 				UMETA(DisplayName = "Breadth First", Tooltip = "Make the dungeon less linear"),
	NbType = 2 				UMETA(Hidden)
};

UENUM()
enum class ESeedType : uint8
{
	Random = 0 				UMETA(DisplayName = "Random", Tooltip = "Random seed at each generation"),
	AutoIncrement = 1 		UMETA(DisplayName = "Auto Increment", Tooltip = "Get the initial seed and increment at each generation"),
	Fixed = 2 				UMETA(DisplayName = "Fixed", Tooltip = "Always use initial seed (or you can set it manually via blueprint)"),
	NbType = 3 				UMETA(Hidden)
};

USTRUCT()
struct FDoorDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "DoorDef")
	FIntVector Position;
	UPROPERTY(EditAnywhere, Category = "DoorDef")
	EDoorDirection Direction;
};
