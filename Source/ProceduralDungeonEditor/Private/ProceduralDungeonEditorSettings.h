/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoomData.h"
#include "ProceduralDungeonEdTypes.h"
#include "ProceduralDungeonEditorSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig)
class UProceduralDungeonEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UProceduralDungeonEditorSettings(const FObjectInitializer& ObjectInitializer);

	// The default RoomData class to use in the class picker when creating a new RoomData asset.
	UPROPERTY(Config, EditAnywhere, Category = "General", NoClear, meta = (AllowAbstract = false))
	TSubclassOf<URoomData> DefaultRoomDataClass;

	// The class picker will not show if the default RoomData class has no child classes
	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (DisplayName = "Use Automatically Default Class If No Child"))
	bool bUseDefaultIfNoChild;

	// The class picker will show only the default RoomData and its children
	//UPROPERTY(Config, EditAnywhere, Category = "General")
	//bool bShowOnlyDefaultAndChildren;

	// Default margin values on each axis to update volumes in Room Editor mode.
	UPROPERTY(Config, EditAnywhere, Category = "Room Editor Mode")
	FMargin3D DefaultMargins;

	// When ticked the orbit rotation of the camera will use the room bounds center as pivot point
	// instead of the current selected actor when the Room Editor Mode is active.
	// (Works only when "Orbit Camera Around Selection" is ticked in your Editor Preferences)
	UPROPERTY(Config, EditAnywhere, Category = "Room Editor Mode", meta = (DisplayName = "Use the room bounds center as pivot for camera orbit"))
	bool bUseRoomAsOrbitPivot {true};
};
