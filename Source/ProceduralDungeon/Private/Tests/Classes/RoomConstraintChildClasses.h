// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "RoomConstraints/RoomConstraint.h"
#include "RoomConstraintChildClasses.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UConstraintPass : public URoomConstraint
{
	GENERATED_BODY()

public:
	virtual bool Check_Implementation(const URoomData* RoomData, FIntVector Location, EDoorDirection Direction) const override
	{
		return true;
	}
};

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class UConstraintFail : public URoomConstraint
{
	GENERATED_BODY()

public:
	virtual bool Check_Implementation(const URoomData* RoomData, FIntVector Location, EDoorDirection Direction) const override
	{
		return false;
	}
};
