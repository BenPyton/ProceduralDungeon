/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ProceduralDungeonCustomVersion.h"
#include "Serialization/CustomVersion.h"

const FGuid FProceduralDungeonCustomVersion::GUID(0x07E63959, 0x72E5DEE1, 0x7B00F72A, 0x1B442349);

// Register the custom version with core
FCustomVersionRegistration GRegisterDungeonCustomVersion(FProceduralDungeonCustomVersion::GUID, FProceduralDungeonCustomVersion::LatestVersion, TEXT("ProcDungeonVer"));
