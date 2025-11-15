// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "RoomConstraints/RoomConstraint.h"
#include "ProceduralDungeonLog.h"

bool URoomConstraint::Check_Implementation(const URoomData* RoomData, FIntVector Location, EDoorDirection Direction) const
{
	DungeonLog_WarningSilent("Constraint %s does not implements the URoomConstraint::Check function!", *GetNameSafe(this));
	return true;
}
