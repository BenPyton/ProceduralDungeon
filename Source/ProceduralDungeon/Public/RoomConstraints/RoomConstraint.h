// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralDungeonTypes.h"
#include "RoomConstraint.generated.h"

class URoomData;

// Base class for constraints applied to rooms
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class PROCEDURALDUNGEON_API URoomConstraint : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Room Constraint")
	bool Check(const URoomData* RoomData, FIntVector Location, EDoorDirection Direction) const;
};
