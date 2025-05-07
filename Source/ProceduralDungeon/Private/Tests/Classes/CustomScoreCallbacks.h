/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "VoxelBounds/VoxelBounds.h"
#include "CustomScoreCallbacks.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UCustomScoreCallback : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	bool ZeroScore(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B, int32& Score)
	{
		Score = 0;
		return true;
	}

	UFUNCTION()
	bool NeverPass(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B, int32& Score)
	{
		return false;
	}
};
