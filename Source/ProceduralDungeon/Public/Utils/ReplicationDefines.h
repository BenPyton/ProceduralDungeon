// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_OLDER_THAN(5, 0, 0)
	// Fixup undefined UE_WITH_IRIS for UE4
	#ifndef UE_WITH_IRIS
		#define UE_WITH_IRIS 0
	#endif

	#define UE_WITH_SUBOBJECT_LIST 0
#else
	#define UE_WITH_SUBOBJECT_LIST 1
#endif
