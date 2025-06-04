// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "ProceduralDungeonTypes.h"
#include "Misc/EngineVersionComparison.h"
#include "Math/GenericOctree.h" // FBoxCenterAndExtent
#include "ProceduralDungeonEdTypes.generated.h"

#if UE_VERSION_OLDER_THAN(5, 0, 0)
	#define COMPATIBILITY 1
#else
	#define COMPATIBILITY 0
#endif

#if !COMPATIBILITY
	#include "UnrealWidgetFwd.h"
#endif

#if COMPATIBILITY
using WidgetMode = FWidget::EWidgetMode;
#else
using WidgetMode = UE::Widget::EWidgetMode;
#endif

#if UE_VERSION_NEWER_THAN(5, 3, 0)
	#define GC_PTR(VAR_TYPE) TObjectPtr<VAR_TYPE>
#else
	#define GC_PTR(VAR_TYPE) VAR_TYPE*
#endif

// Some utility functions for EAxisList
EAxisList::Type operator~(const EAxisList::Type& This);
EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other);

// Holds margin values in 3D (e.g. used for box volumes)
USTRUCT()
struct FMargin3D
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Margin", meta = (DisplayName = "X"))
	FVector2D XAxis {0.0f, 0.0f};

	UPROPERTY(EditAnywhere, Category = "Margin", meta = (DisplayName = "Y"))
	FVector2D YAxis {0.0f, 0.0f};

	UPROPERTY(EditAnywhere, Category = "Margin", meta = (DisplayName = "Z"))
	FVector2D ZAxis {0.0f, 0.0f};

	// Create a new bounds from an existing bounds with the margin applied on it.
	FBoxCenterAndExtent Apply(const FBoxCenterAndExtent& Bounds) const;
};
