/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
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
#include "ProceduralDungeonTypes.h"
#include "BoundsParams.generated.h"

// Holds the settings for the dungeon limits.
// These values are expressed in Room cells, and are based on the origin of the first room (0,0,0).
// For example, if the first room is only 1 room cell (`FirstPoint = (0,0,0)`, `SecondPoint = (1,1,1)`), then  this is the cell (0,0,0).
// If you set a `MinY=2` et `MaxY=2`, then on the Y axis the dungeon can go from the cell -2 to cell 2,
// Making an effective range of 5 cells, centered on the first room.
USTRUCT(BlueprintType, meta = (ShortToolTip = "Holds the settings for the dungeon limits."))
struct FBoundsParams
{
	GENERATED_BODY()

public:
	// Enables the X limit in positive axis (north from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMaxX {false};

	// The X positive limit (north) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMaxX", UIMin = 0, ClampMin = 0))
	int32 MaxX {0};

	// Enables the X limit in negative axis (south from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMinX {false};

	// The X negative limit (south) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMinX", UIMin = 0, ClampMin = 0))
	int32 MinX {0};

	// Enables the Y limit in positive axis (east from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMaxY {false};

	// The Y positive limit (east) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMaxY", UIMin = 0, ClampMin = 0))
	int32 MaxY {0};

	// Enables the Y limit in negative axis (west from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMinY {false};

	// The Y negative limit (west) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMinY", UIMin = 0, ClampMin = 0))
	int32 MinY {0};

	// Enables the Z limit in positive axis (up from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMaxZ {false};

	// The Z positive limit (up) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMaxZ", UIMin = 0, ClampMin = 0))
	int32 MaxZ {0};

	// Enables the Z limit in negative axis (down from the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (InlineEditConditionToggle))
	bool bLimitMinZ {false};

	// The Z negative limit (down) of the dungeon in room units (starting from the origin of the first room).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation|Bounds Limits", meta = (EditCondition = "bLimitMinZ", UIMin = 0, ClampMin = 0))
	int32 MinZ {0};

public:
	FBoxMinAndMax GetBox() const;
};
