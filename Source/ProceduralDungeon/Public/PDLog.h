// Copyright Benoit Pelletier 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"

#if NO_LOGGING
#define _PD_LOG_PrivateImpl(...)
#else
// Private implementation. Dot not use it.
#define _PD_LOG_PrivateImpl(LogCategory, Verbosity, Detailed, Format, ...) \
{ \
	if constexpr (Detailed) \
		{ UE_LOG(LogCategory, Verbosity, TEXT("[%hs:%d] " Format), __FUNCTION__, __LINE__, ##__VA_ARGS__); } \
	else \
		{ UE_LOG(LogCategory, Verbosity, TEXT(Format), ##__VA_ARGS__); } \
}
#endif // NO_LOGGING

// Logs a message to output only
// Must define a LOG_CATEGORY
#define PD_LOG_DEBUG(Format, ...) _PD_LOG_PrivateImpl(LOG_CATEGORY, VeryVerbose, true, Format, ##__VA_ARGS__)
#define PD_LOG_INFO(Format, ...) _PD_LOG_PrivateImpl(LOG_CATEGORY, Log, false, Format, ##__VA_ARGS__)
#define PD_LOG_WARNING(Format, ...) _PD_LOG_PrivateImpl(LOG_CATEGORY, Warning, true, Format, ##__VA_ARGS__)
#define PD_LOG_ERROR(Format, ...) _PD_LOG_PrivateImpl(LOG_CATEGORY, Error, true, Format, ##__VA_ARGS__)
