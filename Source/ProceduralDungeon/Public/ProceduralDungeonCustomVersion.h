// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

// Custom serialization version for Procedural Dungeon
struct FProceduralDungeonCustomVersion
{
	enum Type
	{
		// Before any version changes were made
		InitialVersion = 0,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	PROCEDURALDUNGEON_API const static FGuid GUID;

	FProceduralDungeonCustomVersion() = delete;
};
