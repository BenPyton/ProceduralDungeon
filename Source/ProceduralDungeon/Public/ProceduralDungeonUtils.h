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

#pragma once

#include "Math/Vector.h"
#include "Math/IntVector.h"
#include "Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "Utils/CompatUtils.h"

namespace IntVector
{
	// Returns the component-wise minimum of A and B
	FIntVector PROCEDURALDUNGEON_API Min(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise maximum of A and B
	FIntVector PROCEDURALDUNGEON_API Max(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise minimum and maximum of A and B
	void PROCEDURALDUNGEON_API MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax);
}

class FBoxCenterAndExtent;
struct FBoxMinAndMax;

namespace Dungeon
{
	// Returns the real world location of a point in room units
	FVector PROCEDURALDUNGEON_API ToWorldLocation(FIntVector RoomPoint);

	// Returns the real world vector (no offset) of a vector in room units
	FVector PROCEDURALDUNGEON_API ToWorldVector(FIntVector RoomVector);

	// Convertthe Box from dungeon coordinate to world coordinate, applying an optional transform on it.
	FBoxCenterAndExtent PROCEDURALDUNGEON_API ToWorld(const FBoxMinAndMax& Box, const FTransform& Transform = FTransform::Identity);

	// Convertthe Box from dungeon coordinate to world coordinate, applying an optional transform on it.
	FBoxCenterAndExtent PROCEDURALDUNGEON_API ToWorld(const FBoxCenterAndExtent& Box, const FTransform& Transform = FTransform::Identity);

	// Returns the location in room units from a point in real world
	FIntVector PROCEDURALDUNGEON_API ToRoomLocation(FVector WorldPoint);

	// Returns the vector (no offset) in room units from a vector in real world
	FIntVector PROCEDURALDUNGEON_API ToRoomVector(FVector WorldVector);

	// Returns the real world snapped location to the nearest point in room units from a real world point
	FVector PROCEDURALDUNGEON_API SnapPoint(FVector Point);

	template<typename T>
	int GetTotalWeight(const TMap<T, int>& WeightMap)
	{
		int Total = 0;
		for (const auto& Pair : WeightMap)
		{
			Total += Pair.Value;
		}
		return Total;
	}

	template<typename T>
	T GetWeightedAt(const TMap<T, int>& WeightMap, int Index)
	{
		if (Index < 0)
			return T();

		int Current = 0;
		for (const auto& Pair : WeightMap)
		{
			Current += Pair.Value;
			if (Current > Index)
				return Pair.Key;
		}
		return T();
	}

	// ===== Plugin's Settings =====

	FVector PROCEDURALDUNGEON_API RoomUnit();
	FVector PROCEDURALDUNGEON_API DefaultDoorSize();
	FColor PROCEDURALDUNGEON_API DefaultDoorColor();
	float PROCEDURALDUNGEON_API DoorOffset();
	bool PROCEDURALDUNGEON_API OcclusionCulling();
	bool PROCEDURALDUNGEON_API UseLegacyOcclusion();
	uint32 PROCEDURALDUNGEON_API OcclusionDistance();
	bool PROCEDURALDUNGEON_API OccludeDynamicActors();
	bool PROCEDURALDUNGEON_API DrawDebug();
	bool PROCEDURALDUNGEON_API DrawOnlyWhenEditingRoom();
	bool PROCEDURALDUNGEON_API ShowRoomOrigin();
	bool PROCEDURALDUNGEON_API FlipDoorArrow();
	float PROCEDURALDUNGEON_API DoorArrowLength();
	float PROCEDURALDUNGEON_API DoorArrowHeadSize();
	bool PROCEDURALDUNGEON_API CanLoop();
	ECollisionChannel PROCEDURALDUNGEON_API RoomObjectType();
	uint32 PROCEDURALDUNGEON_API MaxGenerationTryBeforeGivingUp();
	uint32 PROCEDURALDUNGEON_API MaxRoomPlacementTryBeforeGivingUp();
	int32 PROCEDURALDUNGEON_API RoomLimit();

	void PROCEDURALDUNGEON_API EnableOcclusionCulling(bool Enable);
	void PROCEDURALDUNGEON_API SetOcclusionDistance(int32 Distance);
}

namespace Random
{
	uint32 PROCEDURALDUNGEON_API Guid2Seed(FGuid Guid, int64 Salt);
}

namespace WorldUtils
{
	template<class T UE_REQUIRES(TIsDerivedFrom<T, AActor>::Value)>
	void FindAllActors(UWorld* InWorld, TArray<T*>& OutActors)
	{
		OutActors.Empty();
		for (TActorIterator<T> It(InWorld); It; ++It)
		{
			T* Actor = *It;
			OutActors.Add(Actor);
		}
	}

	template<class T UE_REQUIRES(TIsDerivedFrom<T, AActor>::Value)>
	void FindAllActorsByPredicate(UWorld* InWorld, TArray<T*>& OutActors, TFunction<bool(const T*)> Predicate)
	{
		OutActors.Empty();
		for (TActorIterator<T> It(InWorld); It; ++It)
		{
			T* Actor = *It;
			if (Predicate(Actor))
			{
				OutActors.Add(Actor);
			}
		}
	}

	template<typename U, class T UE_REQUIRES(TIsDerivedFrom<T, AActor>::Value)>
	void MapActors(UWorld* InWorld, TMap<U, T*>& OutActorMap, TFunction<U(const T*)> MapFunction)
	{
		OutActorMap.Empty();
		for (TActorIterator<T> It(InWorld); It; ++It)
		{
			T* Actor = *It;
			OutActorMap.Add(MapFunction(Actor), Actor);
		}
	}
}

namespace ObjectUtils
{
	void PROCEDURALDUNGEON_API DispatchToObjectAndSubobjects(UObject* Obj, TFunction<void(UObject*)> Func, int32 Depth = 0);
}
