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
#include "ProceduralDungeonTypes.h"
#include "Room.h" // Access to URoom static values (like URoom::Unit)

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

FBoxCenterAndExtent URoomData::GetBounds(FTransform Transform) const
{
	FVector Center = Transform.TransformPosition(0.5f * URoom::Unit() * FVector(Size - FIntVector(1, 1, 0)));
	FVector Extent = Transform.TransformVector(0.5f * URoom::Unit() * FVector(Size));

	return FBoxCenterAndExtent(Center, Extent.GetAbs());
}

#if WITH_EDITOR

bool URoomData::IsDoorValid(int DoorIndex) const
{
	check(DoorIndex >= 0 && DoorIndex < Doors.Num());

	const FDoorDef& DoorDef = Doors[DoorIndex];

	// Check if door is in the room's bounds
	if ((DoorDef.Position.X < 0 || DoorDef.Position.X >= Size.X)
		|| (DoorDef.Position.Y < 0 || DoorDef.Position.Y >= Size.Y)
		|| (DoorDef.Position.Z < 0 || DoorDef.Position.Z >= Size.Z))
		return false;
	
	// Check if the door is on the edge of the room bounds
	switch (DoorDef.Direction)
	{
	case EDoorDirection::South:
		return DoorDef.Position.X == 0;
	case EDoorDirection::North:
		return DoorDef.Position.X == (Size.X - 1);
	case EDoorDirection::West:
		return DoorDef.Position.Y == 0;
	case EDoorDirection::East:
		return DoorDef.Position.Y == (Size.Y - 1);
	default:
		checkNoEntry();
		return false;
	}
}

EDataValidationResult URoomData::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);
	if (!IsAsset() || Result == EDataValidationResult::Invalid)
		return Result;

	// Check the Level validity
	if (Level.IsNull())
	{
		ValidationErrors.Add(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has no level set. You have to set up a room level."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}

	for (int i = 0; i < Doors.Num(); ++i)
	{
		// Check if all doors are valid
		if (!IsDoorValid(i))
		{
			ValidationErrors.Add(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has invalid door: %s."), *GetName(), *Doors[i].ToString())));
			Result = EDataValidationResult::Invalid;
		}

		// Check if there are no duplicated doors
		for (int k = i + 1; k < Doors.Num(); ++k)
		{
			if (Doors[i] == Doors[k])
			{
				ValidationErrors.Add(FText::FromString(FString::Printf(TEXT("Room data \"%s\" has duplicated doors: %s."), *GetName(), *Doors[i].ToString())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}

#endif // WITH_EDITOR
