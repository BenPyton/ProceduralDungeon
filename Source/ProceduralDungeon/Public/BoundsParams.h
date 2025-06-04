// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
