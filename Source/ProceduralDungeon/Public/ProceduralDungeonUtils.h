// Copyright Benoit Pelletier 2023 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "Math/Vector.h"
#include "Math/IntVector.h"
#include "Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "Utils/CompatUtils.h"

namespace IntVector
{
	// Returns the component-wise minimum of A and B
	PROCEDURALDUNGEON_API FIntVector Min(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise maximum of A and B
	PROCEDURALDUNGEON_API FIntVector Max(const FIntVector& A, const FIntVector& B);

	// Returns the component-wise minimum and maximum of A and B
	PROCEDURALDUNGEON_API void MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax);
} //namespace IntVector

class FBoxCenterAndExtent;
struct FBoxMinAndMax;

namespace Dungeon
{
	// Returns the real world location of a point in room units
	PROCEDURALDUNGEON_API FVector ToWorldLocation(FIntVector RoomPoint, const FVector RoomUnit);

	// Returns the real world vector (no offset) of a vector in room units
	PROCEDURALDUNGEON_API FVector ToWorldVector(FIntVector RoomVector, const FVector RoomUnit);

	// Convert the Box from dungeon coordinate to world coordinate, applying an optional transform on it.
	PROCEDURALDUNGEON_API FBoxCenterAndExtent ToWorld(const FBoxMinAndMax& Box, const FVector RoomUnit, const FTransform& Transform = FTransform::Identity);

	// Convert the Box from dungeon coordinate to world coordinate, applying an optional transform on it.
	PROCEDURALDUNGEON_API FBoxCenterAndExtent ToWorld(const FBoxCenterAndExtent& Box, const FVector RoomUnit, const FTransform& Transform = FTransform::Identity);

	// Returns the location in room units from a point in real world
	PROCEDURALDUNGEON_API FIntVector ToRoomLocation(FVector WorldPoint, const FVector RoomUnit);

	// Returns the vector (no offset) in room units from a vector in real world
	PROCEDURALDUNGEON_API FIntVector ToRoomVector(FVector WorldVector, const FVector RoomUnit);

	// Returns the real world snapped location to the nearest point in room units from a real world point
	PROCEDURALDUNGEON_API FVector SnapPoint(FVector Point, const FVector RoomUnit);

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

	PROCEDURALDUNGEON_API FVector RoomUnit();
	PROCEDURALDUNGEON_API FVector DefaultDoorSize();
	PROCEDURALDUNGEON_API FColor DefaultDoorColor();
	PROCEDURALDUNGEON_API float DoorOffset();
	PROCEDURALDUNGEON_API bool OcclusionCulling();
	PROCEDURALDUNGEON_API bool UseLegacyOcclusion();
	PROCEDURALDUNGEON_API uint32 OcclusionDistance();
	PROCEDURALDUNGEON_API bool OccludeDynamicActors();
	PROCEDURALDUNGEON_API bool DrawDebug();
	PROCEDURALDUNGEON_API bool DrawOnlyWhenEditingRoom();
	PROCEDURALDUNGEON_API bool ShowRoomOrigin();
	PROCEDURALDUNGEON_API bool FlipDoorArrow();
	PROCEDURALDUNGEON_API float DoorArrowLength();
	PROCEDURALDUNGEON_API float DoorArrowHeadSize();
	PROCEDURALDUNGEON_API bool CanLoop();
	PROCEDURALDUNGEON_API ECollisionChannel RoomObjectType();
	PROCEDURALDUNGEON_API uint32 MaxGenerationTryBeforeGivingUp();
	PROCEDURALDUNGEON_API uint32 MaxRoomPlacementTryBeforeGivingUp();
	PROCEDURALDUNGEON_API int32 RoomLimit();

	PROCEDURALDUNGEON_API void EnableOcclusionCulling(bool Enable);
	PROCEDURALDUNGEON_API void SetOcclusionDistance(int32 Distance);
} //namespace Dungeon

namespace Random
{
	PROCEDURALDUNGEON_API uint32 Guid2Seed(FGuid Guid, int64 Salt);
} //namespace Random

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
} //namespace WorldUtils

namespace ObjectUtils
{
	PROCEDURALDUNGEON_API void DispatchToObjectAndSubobjects(UObject* Obj, TFunction<void(UObject*)> Func, int32 Depth = 0);
} //namespace ObjectUtils

namespace ActorUtils
{
	// Returns the bounding box of an actor considering only components that would interact with rooms (based on collision settings).
	PROCEDURALDUNGEON_API FBox GetActorBoundingBoxForRooms(AActor* Actor, const FTransform& DungeonTransform = FTransform::Identity);

	// Returns the player controller associated with the player state id.
	PROCEDURALDUNGEON_API class APlayerController* GetPlayerControllerFromPlayerId(const UObject* WorldContextObject, int32 PlayerId);

	PROCEDURALDUNGEON_API UObject* GetInterfaceImplementer(AActor* Actor, TSubclassOf<UInterface> InterfaceClass);

	template<typename T UE_REQUIRES(TIsDerivedFrom<T, UInterface>::Value)>
	UObject* GetInterfaceImplementer(AActor* Actor)
	{
		return GetInterfaceImplementer(Actor, T::StaticClass());
	}
} //namespace ActorUtils
