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

	// Size of a room unit. Room's size in data assets will express the multiple of this unit size.
	// For example a room size of (5, 10, 1) with a unit size of (100, 100, 400) will result of a real room size of (500, 1000, 400).
	UPROPERTY(EditAnywhere, config, Category = "General", meta = (ClampMin = 0))
	FVector RoomUnit;

	// The bounding size of the doors. It is used only to display the door's blue box when "Draw Debug" is set to true.
	UPROPERTY(EditAnywhere, config, Category = "General", meta = (DisplayName = "Default Door Size", ClampMin = 0))
	FVector DoorSize;

	// The height of the door's base from the room's base (in percentage of the room unit Z)
	UPROPERTY(EditAnywhere, config, Category = "General")
	float DoorOffset;

	// The rooms visibility will be toggled off when the player is not inside it or in a room next to it.
	UPROPERTY(EditAnywhere, config, Category = "Occlusion Culling", meta = (DisplayName = "Enable Occlusion Culling"))
	bool OcclusionCulling;

	// The legacy occlusion culling system only toggles the visibility of the actors in the rooms, keeping the collisions, ticking and all oher things.
	// The new system toggles instead the whole room levels visibility, shutting off the ticking and other things of the actors and the level script.
	//UPROPERTY(EditAnywhere, config, Category = "Procedural Dungeon", meta=(EditCondition="OcclusionCulling"))
	//bool LegacyOcclusion;

	// Defines the number of connected rooms are visible from the player's room (1 means only the room where the player is).
	UPROPERTY(EditAnywhere, config, Category = "Occlusion Culling", meta = (EditCondition = "OcclusionCulling"))
	uint32 OcclusionDistance;

	// Keep track of dynamic actors entering and leaving the room to be able to show/hide them with the room.
	UPROPERTY(EditAnywhere, config, Category = "Occlusion Culling", meta = (EditCondition = "OcclusionCulling"))
	bool OccludeDynamicActors;

	// When true, doors will be connected as long they are at the same place.
	// When false, only the doors between the previous and the new generated room will be connected.
	UPROPERTY(EditAnywhere, config, Category = "General")
	bool CanLoop;

	// Show room and door outlines in editor and development builds
	UPROPERTY(EditAnywhere, config, Category = "Debug")
	bool DrawDebug;

	// Show the room origin in magenta (useless imho but here if anyone find it useful)
	UPROPERTY(EditAnywhere, config, Category = "Debug", meta = (EditCondition = "DrawDebug"))
	bool ShowRoomOrigin;

	// Show some logs on the screen
	UPROPERTY(EditAnywhere, config, Category = "Debug")
	bool OnScreenPrintDebug;

	// Duration of the screen logs
	UPROPERTY(EditAnywhere, config, Category = "Debug", meta = (EditCondition = "OnScreenPrintDebug"))
	float PrintDebugDuration;
};
