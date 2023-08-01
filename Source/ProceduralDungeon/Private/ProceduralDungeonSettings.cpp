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

#include "ProceduralDungeonSettings.h"

UProceduralDungeonSettings::UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RoomUnit = FVector(1000, 1000, 400);
	DoorSize = FVector(40, 640, 400);
	DoorOffset = 0.0f;
	OcclusionCulling = true;
	//LegacyOcclusion = true;
	OcclusionDistance = 2;
	OccludeDynamicActors = false;
	DrawDebug = true;
	ShowRoomOrigin = false;
	OnScreenPrintDebug = false;
	PrintDebugDuration = 60.0f;
	CanLoop = true;
}
