/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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

void URoomData::InitializeRoom_Implementation(URoom* Room, UDungeonGraph* Dungeon) const
{
}

FBoxCenterAndExtent URoomData::GetBounds(FTransform Transform) const
{
	FBoxMinAndMax LocalBounds = GetIntBounds();
	FVector Center = Transform.TransformPosition(0.5f * Dungeon::RoomUnit() * FVector(LocalBounds.Min + LocalBounds.Max - FIntVector(1, 1, 0)));
	FVector Extent = Transform.TransformVector(0.5f * Dungeon::RoomUnit() * FVector(LocalBounds.GetSize()));
	return FBoxCenterAndExtent(Center, Extent.GetAbs());
}

FIntVector URoomData::GetSize() const
{
	return GetIntBounds().GetSize();
}

FBoxMinAndMax URoomData::GetIntBounds() const
{
	return FBoxMinAndMax(FirstPoint, SecondPoint);
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
			for (int k = i + 1; k < Doors.Num(); ++k)
			{
				if (Doors[i] == Doors[k])
				{
					VALIDATION_LOG_ERROR(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has duplicated doors: %s."), *GetName(), *Doors[i].ToString())));
					Result = EDataValidationResult::Invalid;
				}
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
