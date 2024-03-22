/*
 * MIT License
 *
 * Copyright (c) 2023-2024 Benoit Pelletier
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

#include "ProceduralDungeonTypes.h"
#include "Misc/EngineVersionComparison.h"
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

EAxisList::Type operator~(const EAxisList::Type& This)
{
	return static_cast<EAxisList::Type>(EAxisList::All - This);
}

EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other)
{
	This = static_cast<EAxisList::Type>(This & Other);
	return This;
}

// Holds margin values in 3D (e.g. used for box volumes)
USTRUCT()
struct FMargin3f
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FVector2f X;

	UPROPERTY(EditAnywhere)
	FVector2f Y;

	UPROPERTY(EditAnywhere)
	FVector2f Z;

	// Create a new bounds from an existing bounds with the margin applied on it.
	FBoxCenterAndExtent Apply(const FBoxCenterAndExtent& Bounds) const
	{
		FBoxCenterAndExtent NewBounds(Bounds);
		NewBounds.Extent += 0.5f * FVector(X.X + X.Y, Y.X + Y.Y, Z.X + Z.Y);
		NewBounds.Center += 0.5f * FVector(X.X - X.Y, Y.X - Y.Y, Z.X - Z.Y);
		return NewBounds;
	}
};
