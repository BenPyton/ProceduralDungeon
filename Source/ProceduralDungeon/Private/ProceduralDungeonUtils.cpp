/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonSettings.h"
#include "Room.h"

FIntVector IntVector::Min(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z));
}

FIntVector IntVector::Max(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z));
}

void IntVector::MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax)
{
	OutMin = Min(A, B);
	OutMax = Max(A, B);
}

FVector Dungeon::ToWorldLocation(FIntVector RoomPoint)
{
	return Dungeon::RoomUnit() * (FVector(RoomPoint) - FVector(0.5f, 0.5f, 0.0f));
}

FIntVector Dungeon::ToRoomLocation(FVector WorldPoint)
{
	const FVector Unit = Dungeon::RoomUnit();
	const int X = FMath::RoundToInt(0.5f + (WorldPoint.X) / Unit.X);
	const int Y = FMath::RoundToInt(0.5f + (WorldPoint.Y) / Unit.Y);
	const int Z = FMath::RoundToInt((WorldPoint.Z) / Unit.Z);
	return FIntVector(X, Y, Z);
}

FIntVector Dungeon::ToRoomVector(FVector WorldVector)
{
	const FVector Unit = Dungeon::RoomUnit();
	const int X = FMath::RoundToInt(WorldVector.X / Unit.X);
	const int Y = FMath::RoundToInt(WorldVector.Y / Unit.Y);
	const int Z = FMath::RoundToInt(WorldVector.Z / Unit.Z);
	return FIntVector(X, Y, Z);
}

FVector Dungeon::SnapPoint(FVector Point)
{
	return ToWorldLocation(ToRoomLocation(Point));
}

// =================== Plugin's Settings ========================

FVector Dungeon::RoomUnit()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->RoomUnit;
}

FVector Dungeon::DefaultDoorSize()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->DoorSize;
}

float Dungeon::DoorOffset()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->DoorOffset;
}

bool Dungeon::OcclusionCulling()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->OcclusionCulling;
}

bool Dungeon::UseLegacyOcclusion()
{
	//const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	//return Settings->LegacyOcclusion;
	return true;
}

uint32 Dungeon::OcclusionDistance()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->OcclusionDistance;
}

bool Dungeon::OccludeDynamicActors()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->OccludeDynamicActors;
}

bool Dungeon::DrawDebug()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->DrawDebug;
}

bool Dungeon::ShowRoomOrigin()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->ShowRoomOrigin;
}

bool Dungeon::FlipDoorArrow()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->bFlipDoorArrowSide;
}

float Dungeon::DoorArrowLength()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->DoorArrowLength;
}

float Dungeon::DoorArrowHeadSize()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->DoorArrowHeadSize;
}

bool Dungeon::CanLoop()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->CanLoop;
}

ECollisionChannel Dungeon::RoomObjectType()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->RoomObjectType;
}

uint32 Dungeon::MaxGenerationTryBeforeGivingUp()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->MaxGenerationTry;
}

uint32 Dungeon::MaxRoomPlacementTryBeforeGivingUp()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->MaxRoomPlacementTry;
}

int32 Dungeon::RoomLimit()
{
	const UProceduralDungeonSettings* Settings = GetDefault<UProceduralDungeonSettings>();
	return Settings->RoomLimit;
}

void Dungeon::EnableOcclusionCulling(bool Enable)
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	Settings->OcclusionCulling = Enable;
}

void Dungeon::SetOcclusionDistance(int32 Distance)
{
	UProceduralDungeonSettings* Settings = GetMutableDefault<UProceduralDungeonSettings>();
	Settings->OcclusionDistance = Distance;
}
