/*
 * MIT License
 *
 * Copyright (c) 2023-2025 Benoit Pelletier
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
#include "ProceduralDungeonLog.h"
#include "Math/GenericOctree.h" // FBoxCenterAndExtent
#include "ProceduralDungeonTypes.h" // FBoxMinAndMax

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

FVector Dungeon::ToWorldVector(FIntVector RoomPoint)
{
	return Dungeon::RoomUnit() * FVector(RoomPoint);
}

FBoxCenterAndExtent Dungeon::ToWorld(const FBoxMinAndMax& Box, const FTransform& Transform)
{
	return ToWorld(Box.ToCenterAndExtent(), Transform);
}

FBoxCenterAndExtent Dungeon::ToWorld(const FBoxCenterAndExtent& Box, const FTransform& Transform)
{
	const FVector Unit = Dungeon::RoomUnit();
	const FVector Center = Transform.TransformPosition(Unit * Box.Center);
	const FVector Extent = Transform.TransformVector(Unit * Box.Extent).GetAbs();
	return FBoxCenterAndExtent(Center, Extent);
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

uint64 Concat(uint32 A, uint32 B)
{
	return (static_cast<uint64>(A) << 32) | static_cast<uint64>(B);
}

uint32 Random::Guid2Seed(FGuid Guid, int64 Salt)
{
	//// CAUTION: This function must not be modified if not necessary!
	//// Or else it will break behaviour compatibility with previous versions of the plugin!

	// Using PCG-RXS-M-XS found in this paper: https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf (p. 45)

	// Creating a 64bit states from the 128bit Guid and the salt.
	const uint64 Part1 = Concat(Guid.A, Guid.B);
	const uint64 Part2 = Concat(Guid.C, Guid.D);
	uint64 State = Part1 ^ Part2 ^ Salt;

	// Applying PCG-RXS-M-XS to create much more variations from the salt.
	const uint8 Count = State >> 59;	// Extracting the highest 5 bits for the random xorshift below (64-5=59)
	State ^= State >> (5 + Count);		// [RXS] Random xorshift (at least 5 to leave the highest 5 bits untouched)
	State *= 12605985483714917081u;		// [M] Multiplication with a really big odd number
	State ^= State >> 43;				// [XS] Xorshifting 1/3 of the top bits to the 1/3 of the lower bits
	return static_cast<uint32>(State ^ (State >> 32)); // Folding the top half for the result on the bottom half to convert into a 32bit output.
}

void ObjectUtils::DispatchToObjectAndSubobjects(UObject* Obj, TFunction<void(UObject*)> Func, int32 Depth)
{
	// Calls the function on the object itself.
	DungeonLog_Debug("[%d] - Dispatch function to object '%s'.", Depth, *GetNameSafe(Obj));
	Func(Obj);

	// Get all direct subobjects of this object.
	TArray<UObject*> Subobjects;
	GetObjectsWithOuter(Obj, Subobjects, /*bIncludeNestedObjects = */false);

	++Depth;
	// Recursively dispatch to all subobjects found.
	for (UObject* Sub : Subobjects)
	{
		DispatchToObjectAndSubobjects(Sub, Func, Depth);
	}
}
