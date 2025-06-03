/*
 * Copyright (c) 2024-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"
#include "Misc/EngineVersionComparison.h"
#include "UObject/StrongObjectPtr.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)
	#define FLAG_APPLICATION_CONTEXT EAutomationTestFlags::ApplicationContextMask
#else
	#define FLAG_APPLICATION_CONTEXT EAutomationTestFlags_ApplicationContextMask
#endif

// Utility to create a data asset
#define CREATE_DATA_ASSET(VAR_TYPE, VAR_NAME) \
	TStrongObjectPtr<VAR_TYPE> VAR_NAME(NewObject<VAR_TYPE>(GetTransientPackage(), #VAR_NAME))
