/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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
#include "Math/GenericOctree.h" // FBoxCenterAndExtent
#include "ProceduralDungeonTypes.generated.h"

UENUM()
enum class EGenerationState : uint8
{
	Idle					UMETA(DisplayName = "Idle"),
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

bool PROCEDURALDUNGEON_API operator!(const EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator+(const EDoorDirection& A, const EDoorDirection& B);
EDoorDirection PROCEDURALDUNGEON_API operator-(const EDoorDirection& A, const EDoorDirection& B);
// TODO: Don't know how to export these...
EDoorDirection& operator+=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator-=(EDoorDirection& A, const EDoorDirection& B);
EDoorDirection& operator++(EDoorDirection& Direction);
EDoorDirection& operator--(EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator++(EDoorDirection& Direction, int);
EDoorDirection PROCEDURALDUNGEON_API operator--(EDoorDirection& Direction, int);
EDoorDirection PROCEDURALDUNGEON_API operator-(const EDoorDirection& Direction);
EDoorDirection PROCEDURALDUNGEON_API operator~(const EDoorDirection& Direction);
inline EDoorDirection PROCEDURALDUNGEON_API Opposite(const EDoorDirection& Direction) { return ~Direction; }
FIntVector PROCEDURALDUNGEON_API ToIntVector(const EDoorDirection& Direction);
FVector PROCEDURALDUNGEON_API ToVector(const EDoorDirection& Direction);
FQuat PROCEDURALDUNGEON_API ToQuaternion(const EDoorDirection& Direction);
FIntVector PROCEDURALDUNGEON_API Rotate(const FIntVector& Pos, const EDoorDirection& Rot);
FVector PROCEDURALDUNGEON_API Rotate(const FVector& Pos, const EDoorDirection& Rot);

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

// Visibility mode for Room Visibilty Components.
UENUM(BlueprintType, meta = (DisplayName = "Room Visibility"))
enum class EVisibilityMode : uint8
{
	Default			UMETA(DisplayName = "Same As Room"),
	ForceVisible	UMETA(DisplayName = "Force Visible"),
	ForceHidden		UMETA(DisplayName = "Force Hidden"),
	Custom			UMETA(DisplayName = "Custom"),
	NbMode			UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct PROCEDURALDUNGEON_API FDoorDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef")
	FIntVector Position {FIntVector::ZeroValue};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef")
	EDoorDirection Direction {EDoorDirection::North};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorDef", meta = (DisplayThumbnail = false))
	class UDoorType* Type {nullptr};

public:
	bool operator==(const FDoorDef& Other) const;

	static bool AreCompatible(const FDoorDef& A, const FDoorDef& B);

	FVector GetDoorSize() const;
	FString GetTypeName() const;
	FString ToString() const;
	FDoorDef GetOpposite() const;
	FBoxCenterAndExtent GetBounds(bool bIncludeOffset = true) const;

	static FVector GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool includeOffset = true);

#if !UE_BUILD_SHIPPING
	static void DrawDebug(const class UWorld* World, const FColor& Color, const FDoorDef& DoorDef, const FTransform& Transform = FTransform::Identity, bool includeOffset = false, bool isConnected = true);
	static void DrawDebug(const class UWorld* World, const FColor& Color, const FVector& DoorSize, const FIntVector& DoorCell = FIntVector::ZeroValue, const EDoorDirection& DoorRot = EDoorDirection::NbDirection, const FTransform& Transform = FTransform::Identity, bool includeOffset = false, bool isConnected = true);
#endif // !UE_BUILD_SHIPPING
};

// TODO: Use UE built-in TBox<FIntVector> instead?
// The downside of doing that would be the Center and Extent computation that is slightly different...
// Also, the IsInside with another box does not consider coincident faces as inside...
// Also, operators + and += don't mean the same (extending box to include a point instead of shifting the box)...
struct PROCEDURALDUNGEON_API FBoxMinAndMax
{
public:
	FIntVector Min {0};
	FIntVector Max {0};

public:
	FBoxMinAndMax() = default;
	FBoxMinAndMax(const FIntVector& A, const FIntVector& B);

	FIntVector GetSize() const;
	FBoxCenterAndExtent ToCenterAndExtent() const;
	bool IsInside(const FIntVector& Cell) const;
	bool IsInside(const FBoxMinAndMax& Other) const;
	void Rotate(const EDoorDirection& Rot);
	FString ToString() const;

	static bool Overlap(const FBoxMinAndMax& A, const FBoxMinAndMax& B);

	FBoxMinAndMax& operator+=(const FIntVector& X);
	FBoxMinAndMax& operator-=(const FIntVector& X);
	FBoxMinAndMax operator+(const FIntVector& X) const;
	FBoxMinAndMax operator-(const FIntVector& X) const;
	bool operator==(const FBoxMinAndMax& Other) const;
	bool operator!=(const FBoxMinAndMax& Other) const;
};

FBoxMinAndMax PROCEDURALDUNGEON_API Rotate(const FBoxMinAndMax& Box, const EDoorDirection& Rot);
