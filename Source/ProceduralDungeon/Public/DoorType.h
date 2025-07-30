// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DoorType.generated.h"

// Data asset to define a door type.
// A door type is used to define the size, offset, and color of a door bounds.
// Doors with different types are not compatible with each others.
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDoorType : public UDataAsset
{
	GENERATED_BODY()

public:
	UDoorType();

	// Returns the door size from the door type asset,
	// or the default door size in plugin's settings if no door type defined.
	static FVector GetSize(const UDoorType* DoorType);

	// Returns the door offset from the door type asset,
	// or the default door offset in plugin's settings if no door type defined.
	static float GetOffset(const UDoorType* DoorType);

	// Returns the door color from the door type asset,
	// or the default door color in plugin's settings if no door type defined.
	static FColor GetColor(const UDoorType* DoorType);
	
	// Returns true if one of the door type is explicitely set to be compatible with the other.
	static bool AreCompatible(const UDoorType* A, const UDoorType* B);

protected:
	// Size of the door bounds, only used by the debug draw as a visual hint for designers and artists.
	UPROPERTY(EditInstanceOnly, Category = "Door Type", meta = (ClampMin = 0))
	FVector Size;

	// The offset of the door bounds from the room's base (in percentage of the room unit Z).
	UPROPERTY(EditInstanceOnly, Category = "Door Type", meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
	float Offset;

	// The color used to draw the door bounds in the editor.
	UPROPERTY(EditInstanceOnly, Category = "Door Type")
	FColor Color;

#if WITH_EDITORONLY_DATA
	// Just a description, used nowhere.
	UPROPERTY(EditInstanceOnly, Category = "Door Type")
	FText Description;
#endif

	// Can this door type be connected with itself?
	UPROPERTY(EditInstanceOnly, Category = "Door Type")
	bool bCompatibleWithItself;

	// Which door types are compatible with this one
	UPROPERTY(EditInstanceOnly, Category = "Door Type")
	TArray<UDoorType*> Compatibility;
};
