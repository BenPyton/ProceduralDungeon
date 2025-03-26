/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
