// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "Misc/EngineVersionComparison.h"
#include "Templates/UnrealTypeTraits.h"

#if UE_VERSION_OLDER_THAN(5, 0, 0)
	#define UE_REQUIRES(...) \
		, typename = typename TEnableIf<__VA_ARGS__>::Type
#elif UE_VERSION_OLDER_THAN(5, 3, 0)
	#define UE_REQUIRES(...)       \
		UE_CONSTRAINTS_BEGIN       \
		UE_CONSTRAINT(__VA_ARGS__) \
		UE_CONSTRAINTS_END
#endif

#if UE_VERSION_OLDER_THAN(5, 0, 0)
	#define AR_FIELD_NAME(Name) FArchiveFieldName(TEXT(Name))
#else
	#define AR_FIELD_NAME(Name) TEXT(Name)
#endif
