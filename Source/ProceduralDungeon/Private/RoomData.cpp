/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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
	FBoxCenterAndExtent Bounds = GetIntBounds().ToCenterAndExtent();
	Bounds.Center = Transform.TransformPosition(Dungeon::RoomUnit() * Bounds.Center);
	Bounds.Extent = Transform.TransformVector(Dungeon::RoomUnit() * Bounds.Extent).GetAbs();
	return Bounds;
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
}

#endif // WITH_EDITOR
