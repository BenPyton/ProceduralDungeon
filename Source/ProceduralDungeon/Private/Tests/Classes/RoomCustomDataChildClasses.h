// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "RoomCustomData.h"
#include "RoomCustomDataChildClasses.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UCustomDataA : public URoomCustomData
{
	GENERATED_BODY()
};

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UCustomDataB : public URoomCustomData
{
	GENERATED_BODY()
};

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UCustomDataC : public URoomCustomData
{
	GENERATED_BODY()
};
