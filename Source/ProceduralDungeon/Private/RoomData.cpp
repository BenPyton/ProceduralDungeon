/*
 * Copyright (c) 2019-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "RoomData.h"
#include "RoomLevel.h"
#include "RoomCustomData.h"
#include "ProceduralDungeonTypes.h"
#include "ProceduralDungeonUtils.h"
#include "DoorType.h"
#include "Math/GenericOctree.h" // FBoxCenterAndExtent

#if !USE_LEGACY_DATA_VALIDATION
	#include "Misc/DataValidation.h"
#endif

URoomData::URoomData()
	: Super()
{
}

const FDoorDef& URoomData::GetDoorDef(int32 DoorIndex) const
{
	if (DoorIndex >= 0 && DoorIndex < Doors.Num())
		return Doors[DoorIndex];
	return FDoorDef::Invalid;
}

bool URoomData::HasCompatibleDoor(const FDoorDef& DoorData) const
{
	for (int i = 0; i < Doors.Num(); ++i)
	{
		if (FDoorDef::AreCompatible(Doors[i], DoorData))
			return true;
	}
	return false;
}

void URoomData::GetCompatibleDoors(const FDoorDef& DoorData, TArray<int>& CompatibleDoors) const
{
	CompatibleDoors.Empty();
	for (int i = 0; i < Doors.Num(); ++i)
	{
		if (FDoorDef::AreCompatible(Doors[i], DoorData))
			CompatibleDoors.Add(i);
	}
}

bool URoomData::HasDoorOfType(UDoorType* DoorType) const
{
	for (const auto& Door : Doors)
	{
		if (Door.Type == DoorType)
			return true;
	}
	return false;
}

bool URoomData::HasAnyDoorOfType(const TArray<UDoorType*>& DoorTypes) const
{
	for (const auto& Door : Doors)
	{
		if (DoorTypes.Contains(Door.Type))
			return true;
	}
	return false;
}

bool URoomData::HasAllDoorOfType(const TArray<UDoorType*>& DoorTypes) const
{
	TSet<UDoorType*> AllDoorTypes(DoorTypes);
	for (const auto& Door : Doors)
	{
		AllDoorTypes.Remove(Door.Type);
	}
	return AllDoorTypes.Num() <= 0;
}

bool URoomData::HasCustomData(TSubclassOf<URoomCustomData> CustomDataClass) const
{
	return CustomData.Contains(CustomDataClass);
}

bool URoomData::HasAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomDataList) const
{
	for (const auto& CustomDataClass : CustomDataList)
	{
		if (HasCustomData(CustomDataClass))
			return true;
	}
	return false;
}

bool URoomData::HasAllCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomDataList) const
{
	for (const auto& CustomDataClass : CustomDataList)
	{
		if (!HasCustomData(CustomDataClass))
			return false;
	}
	return true;
}

void URoomData::InitializeRoom_Implementation(URoom* Room, UDungeonGraph* Dungeon) const
{
}

void URoomData::CleanupRoom_Implementation(URoom* Room, UDungeonGraph* Dungeon) const
{
}

FBoxCenterAndExtent URoomData::GetBounds(FTransform Transform) const
{
	return Dungeon::ToWorld(GetIntBounds(), Transform);
}

FIntVector URoomData::GetSize() const
{
	return GetIntBounds().GetSize();
}

int URoomData::GetVolume() const
{
	FIntVector Size = GetSize();
	return Size.X * Size.Y * Size.Z;
}

FBoxMinAndMax URoomData::GetIntBounds() const
{
	return FBoxMinAndMax(FirstPoint, SecondPoint);
}

FVoxelBounds URoomData::GetVoxelBounds() const
{
	if (CachedVoxelBounds.IsValid())
		return CachedVoxelBounds;

	// For now, just convert the IntBounds into a VoxelBounds.
	// When the VoxelBounds editor will be implemented, we will just have to return the serialized VoxelBounds.
	FBoxMinAndMax Bounds = GetIntBounds();
	for (int X = Bounds.Min.X; X < Bounds.Max.X; ++X)
	{
		for (int Y = Bounds.Min.Y; Y < Bounds.Max.Y; ++Y)
		{
			for (int Z = Bounds.Min.Z; Z < Bounds.Max.Z; ++Z)
			{
				CachedVoxelBounds.AddCell(FIntVector(X, Y, Z));
			}
		}
	}

	const FVoxelBoundsConnection WallConnection(FVoxelBoundsConnection::EType::Wall);

	// Fill top and bottom with walls.
	for (int X = Bounds.Min.X; X < Bounds.Max.X; ++X)
	{
		for (int Y = Bounds.Min.Y; Y < Bounds.Max.Y; ++Y)
		{
			CachedVoxelBounds.SetCellConnection(FIntVector(X, Y, Bounds.Min.Z), FVoxelBounds::EDirection::Down, WallConnection);
			CachedVoxelBounds.SetCellConnection(FIntVector(X, Y, Bounds.Max.Z - 1), FVoxelBounds::EDirection::Up, WallConnection);
		}
	}

	// Fill left and right with walls.
	for (int Y = Bounds.Min.Y; Y < Bounds.Max.Y; ++Y)
	{
		for (int Z = Bounds.Min.Z; Z < Bounds.Max.Z; ++Z)
		{
			CachedVoxelBounds.SetCellConnection(FIntVector(Bounds.Min.X, Y, Z), FVoxelBounds::EDirection::West, WallConnection);
			CachedVoxelBounds.SetCellConnection(FIntVector(Bounds.Max.X - 1, Y, Z), FVoxelBounds::EDirection::East, WallConnection);
		}
	}

	// Fill front and back with walls.
	for (int X = Bounds.Min.X; X < Bounds.Max.X; ++X)
	{
		for (int Z = Bounds.Min.Z; Z < Bounds.Max.Z; ++Z)
		{
			CachedVoxelBounds.SetCellConnection(FIntVector(X, Bounds.Min.Y, Z), FVoxelBounds::EDirection::South, WallConnection);
			CachedVoxelBounds.SetCellConnection(FIntVector(X, Bounds.Max.Y - 1, Z), FVoxelBounds::EDirection::North, WallConnection);
		}
	}

	// Add the doors
	for (int i = 0; i < Doors.Num(); ++i)
	{
		const FDoorDef& Door = Doors[i];
		const FIntVector DoorPos = Door.Position;
		const EDoorDirection DoorDir = Door.Direction;
		CachedVoxelBounds.SetCellConnection(DoorPos, FVoxelBounds::EDirection(DoorDir), FVoxelBoundsConnection(Door.Type));
	}

	return CachedVoxelBounds;
}

bool URoomData::IsRoomInBounds(const FBoxMinAndMax& Bounds, int DoorIndex, const FDoorDef& DoorDungeonPos) const
{
	const FIntVector BoundSize = Bounds.GetSize();
	if (BoundSize.X == 0 || BoundSize.Y == 0 || BoundSize.Z == 0)
		return false;

	if (DoorIndex < 0 || DoorIndex >= Doors.Num())
		return false;

	const FDoorDef& Door = Doors[DoorIndex];
	FBoxMinAndMax RoomBounds = GetIntBounds();
	RoomBounds -= Door.Position;
	RoomBounds.Rotate(DoorDungeonPos.Direction - Door.Direction);
	RoomBounds += DoorDungeonPos.Position;
	return Bounds.IsInside(RoomBounds);
}

#if !(UE_BUILD_SHIPPING) || WITH_EDITOR

bool URoomData::IsDoorValid(int DoorIndex) const
{
	check(DoorIndex >= 0 && DoorIndex < Doors.Num());

	const FDoorDef& DoorDef = Doors[DoorIndex];

	FIntVector Min = IntVector::Min(FirstPoint, SecondPoint);
	FIntVector Max = IntVector::Max(FirstPoint, SecondPoint);

	// Check if door is in the room's bounds
	if ((DoorDef.Position.X < Min.X || DoorDef.Position.X >= Max.X)
		|| (DoorDef.Position.Y < Min.Y || DoorDef.Position.Y >= Max.Y)
		|| (DoorDef.Position.Z < Min.Z || DoorDef.Position.Z >= Max.Z))
		return false;

	// Check if the door is on the edge of the room bounds
	switch (DoorDef.Direction)
	{
	case EDoorDirection::South:
		return DoorDef.Position.X == Min.X;
	case EDoorDirection::North:
		return DoorDef.Position.X == (Max.X - 1);
	case EDoorDirection::West:
		return DoorDef.Position.Y == Min.Y;
	case EDoorDirection::East:
		return DoorDef.Position.Y == (Max.Y - 1);
	default:
		checkNoEntry();
		return false;
	}
}

bool URoomData::IsDoorDuplicate(int DoorIndex) const
{
	check(DoorIndex >= 0 && DoorIndex < Doors.Num());
	for (int i = 0; i < Doors.Num(); ++i)
	{
		if (DoorIndex != i && Doors[i] == Doors[DoorIndex])
			return true;
	}
	return false;
}

#endif // !(UE_BUILD_SHIPPING) || WITH_EDITOR

#if WITH_EDITOR

	#if USE_LEGACY_DATA_VALIDATION
		#define VALIDATION_LOG_ERROR(Msg) ValidationErrors.Add(Msg)
EDataValidationResult URoomData::IsDataValid(TArray<FText>& ValidationErrors)
	#else
		#define VALIDATION_LOG_ERROR(Msg) Context.AddError(Msg)
EDataValidationResult URoomData::IsDataValid(FDataValidationContext& Context) const
	#endif // USE_LEGACY_DATA_VALIDATION
{
	#if USE_LEGACY_DATA_VALIDATION
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);
	#else
	EDataValidationResult Result = Super::IsDataValid(Context);
	#endif // USE_LEGACY_DATA_VALIDATION
	if (!IsAsset() || Result == EDataValidationResult::Invalid)
		return Result;

	// Check the Level validity
	if (Level.IsNull())
	{
		VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has no level set. You have to set up a room level."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}

	// Check if no room size is 0 on any axis
	if (FirstPoint.X == SecondPoint.X
		|| FirstPoint.Y == SecondPoint.Y
		|| FirstPoint.Z == SecondPoint.Z)
	{
		VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has a size of 0 on at least one axis."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}

	if (Doors.Num() <= 0)
	{
		VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" should have at least one door."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		for (int i = 0; i < Doors.Num(); ++i)
		{
			// Check if all doors are valid
			if (!IsDoorValid(i))
			{
				VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has invalid door: %s."), *GetName(), *Doors[i].ToString())));
				Result = EDataValidationResult::Invalid;
			}

			// Check if there are no duplicated doors
			if (IsDoorDuplicate(i))
			{
				VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has duplicated doors: %s."), *GetName(), *Doors[i].ToString())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	// Check if CustomData Set does not have null value
	if (CustomData.Contains(nullptr))
	{
		VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has value None in CustomData."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
	#undef VALIDATION_LOG_ERROR

void URoomData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnPropertiesChanged.Broadcast(this);

	// Reset the cached VoxelBounds to trigger a new computation.
	CachedVoxelBounds = FVoxelBounds();
}

#endif // WITH_EDITOR
