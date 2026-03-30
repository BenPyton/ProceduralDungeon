// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "RoomData.h"
#include "RoomLevel.h"
#include "RoomCustomData.h"
#include "ProceduralDungeonTypes.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "ProceduralDungeonCustomVersion.h"
#include "DoorType.h"
#include "DungeonSettings.h"
#include "RoomConstraints/RoomConstraint.h"
#include "Serialization/CustomVersion.h"

#if !USE_LEGACY_DATA_VALIDATION
	#include "Misc/DataValidation.h"
#endif

URoomData::URoomData()
	: Super()
{
	BoundingBoxes.Add({FIntVector(0), FIntVector(1)});
}

void URoomData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FProceduralDungeonCustomVersion::GUID);

	// If loading an old version, we need to handle the migration
	if (Ar.IsLoading())
	{
		const int32 DungeonVersion = Ar.CustomVer(FProceduralDungeonCustomVersion::GUID);
		
		if (DungeonVersion < FProceduralDungeonCustomVersion::RoomDataBoundingBoxesMigration)
		{
			DungeonLog_Warning("Migrating RoomData '%s' from legacy FirstPoint/SecondPoint to BoundingBoxes.", *GetName());

			if (BoundingBoxes.Num() == 0)
				BoundingBoxes.AddDefaulted();
				
			BoundingBoxes[0].SetMinAndMax(FirstPoint, SecondPoint);
				
			// Clear the legacy data after migration
			FirstPoint = FIntVector(0);
			SecondPoint = FIntVector(0);
		}
	}
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

FVector URoomData::GetRoomUnit() const
{
	return UDungeonSettings::GetRoomUnit(GetSettings());
}

bool URoomData::DoesPassAllConstraints(const UDungeonGraph* Dungeon, const URoomData* RoomData, FIntVector Location, EDoorDirection Direction)
{
	if (!IsValid(RoomData))
	{
		return false;
	}

	for (const URoomConstraint* Constraint : RoomData->Constraints)
	{
		if (!IsValid(Constraint))
		{
			DungeonLog_WarningSilent("Invalid constraint detected in RoomData %s", *GetNameSafe(RoomData));
			continue;
		}

		if (!Constraint->Check(Dungeon, RoomData, Location, Direction))
			return false;
	}
	return true;
}

FBoxCenterAndExtent URoomData::GetBounds(FTransform Transform) const
{
	return Dungeon::ToWorld(GetIntBounds(), GetRoomUnit(), Transform);
}

FBoxCenterAndExtent URoomData::GetSubBounds(int32 Index, FTransform Transform) const
{
	check(Index >= 0 && Index < BoundingBoxes.Num());
	const FBoxMinAndMax& Box = BoundingBoxes[Index];
	return Dungeon::ToWorld(Box, GetRoomUnit(), Transform);
}

FIntVector URoomData::GetSize() const
{
	return GetIntBounds().GetSize();
}

int URoomData::GetVolume() const
{
	const FVoxelBounds Bounds = GetVoxelBounds();
	return Bounds.GetCellCount();
}

FBoxMinAndMax URoomData::GetIntBounds() const
{
	return GetVoxelBounds().GetBounds();
}

FVoxelBounds URoomData::GetVoxelBounds() const
{
	if (CachedVoxelBounds.IsValid())
		return CachedVoxelBounds;

	// For now, just convert the IntBounds into a VoxelBounds.
	// When the VoxelBounds editor will be implemented, we will just have to return the serialized VoxelBounds.
	for (const FBoxMinAndMax& Box : BoundingBoxes)
	{
		CachedVoxelBounds.AddBox(Box);
	}
	CachedVoxelBounds.ResetToWalls();

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

	bool bFacingNoBox = true;
	bool bAtLeastInABox = false;
	const FDoorDef& DoorDef = Doors[DoorIndex];
	for (const auto& Box : BoundingBoxes)
	{
		bAtLeastInABox |= Box.IsInside(DoorDef.Position);

		const FIntVector FacingCell = DoorDef.Position + ToIntVector(DoorDef.Direction);
		bFacingNoBox &= !Box.IsInside(FacingCell);
	}

	return bAtLeastInABox && bFacingNoBox;
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

void URoomData::DrawDebug(const UWorld* World, const FTransform& Transform, const FColor& Color)
{
	if (!IsValid(World))
		return;

	for (const FBoxMinAndMax& BoundingBox : BoundingBoxes)
	{
		const FBoxCenterAndExtent Box = Dungeon::ToWorld(BoundingBox, GetRoomUnit(), Transform);
		DrawDebugBox(World, Box.Center, Box.Extent, FQuat::Identity, Color, false, -1.0f, SDPG_World, 2.0f);
	}
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

	if (BoundingBoxes.Num() <= 0)
	{
		VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" should have at least one bounding box."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		// Check if all bounding boxes are valid
		for (const FBoxMinAndMax& Box : BoundingBoxes)
		{
			if (!Box.IsValid())
			{
				VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has an invalid bounding box: %s."), *GetName(), *Box.ToString())));
				Result = EDataValidationResult::Invalid;
			}
		}
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
