// Copyright Benoit Pelletier 2023 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "DungeonOctree.h"
#include "Room.h"

FDungeonOctreeElement::FDungeonOctreeElement(URoom* Room, int32 BoxIndex)
{
	check(IsValid(Room));
	this->Room = Room;
	Bounds = Room->GetSubBounds(BoxIndex);

	// Shrink the bounds slightly to avoid overlapping with other rooms in the octree when sharing the same face.
	// Also forces W to 0 to avoid issues with octree intersection tests.
	Bounds.Extent -= FVector(0.0001f);
	Bounds.Extent.W = 0;
}
