/*
 * MIT License
 *
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
