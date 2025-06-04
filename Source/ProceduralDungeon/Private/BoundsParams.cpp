// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "BoundsParams.h"

FBoxMinAndMax FBoundsParams::GetBox() const
{
	return FBoxMinAndMax(
		FIntVector(
			(bLimitMinX) ? -MinX : INT32_MIN,
			(bLimitMinY) ? -MinY : INT32_MIN,
			(bLimitMinZ) ? -MinZ : INT32_MIN
		),
		FIntVector(
			(bLimitMaxX) ? MaxX + 1 : INT32_MAX,
			(bLimitMaxY) ? MaxY + 1 : INT32_MAX,
			(bLimitMaxZ) ? MaxZ + 1 : INT32_MAX
		)
	);
}
