// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonSettings.generated.h"

// Data asset to allow overriding the plugin's settings
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDungeonSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UDungeonSettings();

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings")
	static FVector GetRoomUnit(const UDungeonSettings* Settings = nullptr);

private:
	// Size of a room unit. Room's size in data assets will express the multiple of this unit size.
	// For example a room size of (5, 10, 1) with a unit size of (100, 100, 400) will result of a real room size of (500, 1000, 400).
	UPROPERTY(EditAnywhere, Category = "General", meta = (ClampMin = 0, AllowPrivateAccess=true))
	FVector RoomUnit;
};
