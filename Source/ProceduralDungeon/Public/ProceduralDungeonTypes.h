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
