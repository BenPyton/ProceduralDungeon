/*
 * MIT License
 *
 * Copyright (c) 2019-2021, 2023 Benoit Pelletier
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
#include "ProceduralDungeonSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class PROCEDURALDUNGEON_API UProceduralDungeonSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	FVector RoomUnit;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	FVector DoorSize;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	float DoorOffset;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon", meta=(Tooltip = "The rooms visibility will be toggled off when the player is not inside it or in a room next to it."))
	bool OcclusionCulling;

	//UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon", meta=(EditCondition="OcclusionCulling", Tooltip = "The legacy occlusion culling system only toggles the visibility of the actors in the rooms, keeping the collisions, ticking and all oher things.\nThe new system toggles instead the whole room levels visibility, shutting off the ticking and other things of the actors and the level script."))
	//bool LegacyOcclusion;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	uint32 OcclusionDistance;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool CanLoop;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool DrawDebug;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	bool OnScreenPrintDebug;

	UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon")
	float PrintDebugDuration;
};
