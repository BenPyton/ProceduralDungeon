/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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
	Play					UMETA(DisplayName = "Play"),
	NbState					UMETA(Hidden)
};

UENUM(BlueprintType, meta = (DisplayName = "Door Direction"))
enum class EDoorDirection : uint8
{
	North		= 0 		UMETA(DisplayName = "North", ToolTip = "rotation = 0 | positive X (world forward)"),
	East		= 1 		UMETA(DisplayName = "East", ToolTip = "rotation = 90 | positive Y (world right)"),
	South		= 2 		UMETA(DisplayName = "South", ToolTip = "rotation = 180 | negative X (world backward)"),
	West		= 3 		UMETA(DisplayName = "West", ToolTip = "rotation = 270 | negative Y (world left)"),
	NbDirection	= 4 		UMETA(Hidden)
};

bool operator!(const EDoorDirection& Direction);
EDoorDirection operator+(const EDoorDirection& A, const EDoorDirection& B);
EDoorDirection operator-(const EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator+=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator-=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator++(EDoorDirection& Direction);
EDoorDirection& operator--(EDoorDirection& Direction);
EDoorDirection operator++(EDoorDirection& Direction, int);
EDoorDirection operator--(EDoorDirection& Direction, int);
EDoorDirection operator-(const EDoorDirection& Direction);
EDoorDirection operator~(const EDoorDirection& Direction);
inline EDoorDirection Opposite(const EDoorDirection& Direction) { return ~Direction; }
FIntVector ToIntVector(const EDoorDirection& Direction);
FVector ToVector(const EDoorDirection& Direction);
FQuat ToQuaternion(const EDoorDirection& Direction);
FIntVector Rotate(const FIntVector& Pos, const EDoorDirection& Rot);

UENUM(BlueprintType, meta = (DisplayName = "Generation Type"))
enum class EGenerationType : uint8
{
	DFS = 0 				UMETA(DisplayName = "Depth First", Tooltip = "Make the dungeon more linear"),
	BFS = 1 				UMETA(DisplayName = "Breadth First", Tooltip = "Make the dungeon less linear"),
	NbType = 2 				UMETA(Hidden)
};

UENUM(BlueprintType, meta = (DisplayName = "Seed Type"))
enum class ESeedType : uint8
{
	Random = 0 				UMETA(DisplayName = "Random", Tooltip = "Random seed at each generation"),
	AutoIncrement = 1 		UMETA(DisplayName = "Auto Increment", Tooltip = "Get the initial seed and increment at each generation"),
	Fixed = 2 				UMETA(DisplayName = "Fixed", Tooltip = "Always use initial seed (or you can set it manually via blueprint)"),
	NbType = 3 				UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FDoorDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef")
	FIntVector Position {FIntVector::ZeroValue};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef")
	EDoorDirection Direction {EDoorDirection::North};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef", meta = (DisplayThumbnail = false))
	class UDoorType* Type;

public:
	bool operator==(const FDoorDef& Other) const
	{
		return Position == Other.Position && Direction == Other.Direction;
	}

	static bool AreCompatible(const FDoorDef& A, const FDoorDef& B)
	{
		return A.Type == B.Type;
	}

	FVector GetDoorSize() const;

	FString ToString() const
	{
		FText DirectionName;
		UEnum::GetDisplayValueAsText(Direction, DirectionName);
		return FString::Printf(TEXT("(%d,%d,%d) [%s]"), Position.X, Position.Y, Position.Z, *DirectionName.ToString());
	}
};

