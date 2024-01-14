/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "Math/GenericOctree.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_OLDER_THAN(4, 26, 0)
#define USE_LEGACY_OCTREE 1
#else
#define USE_LEGACY_OCTREE 0
#endif

struct FDungeonOctreeElement
{
	class URoom* Room;
	FBoxCenterAndExtent Bounds;

	FDungeonOctreeElement(URoom* Room);
};

struct FDungeonOctreeSemantics
{
	enum { MaxElementsPerLeaf = 16 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	FORCEINLINE static const FBoxCenterAndExtent& GetBoundingBox(const FDungeonOctreeElement& Element)
	{
		return Element.Bounds;
	}

	FORCEINLINE static bool AreElementsEqual(const FDungeonOctreeElement& A, const FDungeonOctreeElement& B)
	{
		return A.Room == B.Room;
	}

	FORCEINLINE static void SetElementId(const FDungeonOctreeElement& Element
#if USE_LEGACY_OCTREE
		, FOctreeElementId Id)
#else
		, FOctreeElementId2 Id)
#endif
	{
	}

	FORCEINLINE static void ApplyOffset(FDungeonOctreeElement& Element, FVector Offset)
	{
		Element.Bounds.Center += Offset;
	}
};

using FDungeonOctree =
#if USE_LEGACY_OCTREE
	TOctree<FDungeonOctreeElement, FDungeonOctreeSemantics>;
#else
	TOctree2<FDungeonOctreeElement, FDungeonOctreeSemantics>;
#endif

template<typename IterateBoundsFunc>
inline void FindElementsWithBoundsTest(const FDungeonOctree& Octree, const FBoxCenterAndExtent& Bounds, const IterateBoundsFunc& Func)
{
#if USE_LEGACY_OCTREE
	for (FDungeonOctree::TConstElementBoxIterator<> OctreeIt(Octree, Bounds); OctreeIt.HasPendingElements(); OctreeIt.Advance())
	{
		const FDungeonOctreeElement& Element = OctreeIt.GetCurrentElement();
		Func(Element);
	}
#else
	Octree.FindElementsWithBoundsTest(Bounds, Func);
#endif
}
