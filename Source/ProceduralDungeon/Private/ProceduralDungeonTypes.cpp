/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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

#include "ProceduralDungeonTypes.h"
#include "DrawDebugHelpers.h"
#include "ProceduralDungeonUtils.h"
#include "DoorType.h"

bool operator!(const EDoorDirection& Direction)
{
	return Direction == EDoorDirection::NbDirection;
}

EDoorDirection operator+(const EDoorDirection& A, const EDoorDirection& B)
{
	check(!!A && !!B);
	return static_cast<EDoorDirection>(static_cast<uint8>((static_cast<uint8>(A) + static_cast<uint8>(B))) % static_cast<uint8>(EDoorDirection::NbDirection));
}

EDoorDirection operator-(const EDoorDirection& A, const EDoorDirection& B)
{
	check(!!A && !!B);
	return static_cast<EDoorDirection>(static_cast<uint8>((static_cast<uint8>(A) - static_cast<uint8>(B))) % static_cast<uint8>(EDoorDirection::NbDirection));
}

EDoorDirection& operator+=(EDoorDirection& A, const EDoorDirection& B)
{
	A = A + B;
	return A;
}

EDoorDirection& operator-=(EDoorDirection& A, const EDoorDirection& B)
{
	A = A - B;
	return A;
}

EDoorDirection& operator++(EDoorDirection& Direction)
{
	Direction = Direction + EDoorDirection::East;
	return Direction;
}

EDoorDirection& operator--(EDoorDirection& Direction)
{
	Direction = Direction - EDoorDirection::East;
	return Direction;
}

EDoorDirection operator++(EDoorDirection& Direction, int)
{
	EDoorDirection old {Direction};
	++Direction;
	return old;
}

EDoorDirection operator--(EDoorDirection& Direction, int)
{
	EDoorDirection old {Direction};
	--Direction;
	return old;
}

EDoorDirection operator-(const EDoorDirection& Direction)
{
	return EDoorDirection::North - Direction;
}

EDoorDirection operator~(const EDoorDirection& Direction)
{
	return Direction + EDoorDirection::South;
}

FIntVector ToIntVector(const EDoorDirection& Direction)
{
	FIntVector Dir = FIntVector::ZeroValue;
	switch (Direction)
	{
	case EDoorDirection::North:
		Dir.X = 1;
		break;
	case EDoorDirection::East:
		Dir.Y = 1;
		break;
	case EDoorDirection::West:
		Dir.Y = -1;
		break;
	case EDoorDirection::South:
		Dir.X = -1;
		break;
	default:
		checkNoEntry();
	}
	return Dir;
}

FVector ToVector(const EDoorDirection& Direction)
{
	FVector Dir = FVector::ZeroVector;
	switch (Direction)
	{
	case EDoorDirection::North:
		Dir.X = 1.0f;
		break;
	case EDoorDirection::East:
		Dir.Y = 1.0f;
		break;
	case EDoorDirection::West:
		Dir.Y = -1.0f;
		break;
	case EDoorDirection::South:
		Dir.X = -1.0f;
		break;
	default:
		checkNoEntry();
	}
	return Dir;
}

FQuat ToQuaternion(const EDoorDirection& Direction)
{
	check(!!Direction);
	return FRotator(0.0f, 90.0f * static_cast<uint8>(Direction), 0.0f).Quaternion();
}

FIntVector Rotate(const FIntVector& Pos, const EDoorDirection& Rot)
{
	FIntVector NewPos = Pos;
	switch (Rot)
	{
	case EDoorDirection::North:
		NewPos = Pos;
		break;
	case EDoorDirection::West:
		NewPos.Y = -Pos.X;
		NewPos.X = Pos.Y;
		break;
	case EDoorDirection::East:
		NewPos.Y = Pos.X;
		NewPos.X = -Pos.Y;
		break;
	case EDoorDirection::South:
		NewPos.Y = -Pos.Y;
		NewPos.X = -Pos.X;
		break;
	default:
		checkNoEntry();
	}
	return NewPos;
}

// ############ FDoorDef ##############

bool FDoorDef::operator==(const FDoorDef& Other) const
{
	return Position == Other.Position && Direction == Other.Direction;
}

bool FDoorDef::AreCompatible(const FDoorDef& A, const FDoorDef& B)
{
	return A.Type == B.Type;
}

FVector FDoorDef::GetDoorSize() const
{
	return UDoorType::GetSize(Type);
}

FString FDoorDef::GetTypeName() const
{
	return IsValid(Type) ? Type->GetName() : TEXT("Default");
}

FString FDoorDef::ToString() const
{
	FText DirectionName;
	UEnum::GetDisplayValueAsText(Direction, DirectionName);
	return FString::Printf(TEXT("(%d,%d,%d) [%s]"), Position.X, Position.Y, Position.Z, *DirectionName.ToString());
}

FVector FDoorDef::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool includeOffset)
{
	const FVector CellPosition = FVector(DoorCell);
	const FVector DirectionOffset = !DoorRot ? FVector::ZeroVector : (0.5f * ToVector(DoorRot));
	const FVector HeightOffset = includeOffset ? FVector(0, 0, Dungeon::DoorOffset()) : FVector::ZeroVector;
	return Dungeon::RoomUnit() * (CellPosition + DirectionOffset + HeightOffset);
}


void FDoorDef::DrawDebug(const UWorld* World, const FColor& Color, const FDoorDef& DoorDef, const FTransform& Transform, bool includeOffset, bool isConnected)
{
	DrawDebug(World, Color, DoorDef.GetDoorSize(), DoorDef.Position, DoorDef.Direction, Transform, includeOffset, isConnected);
}

void FDoorDef::DrawDebug(const UWorld* World, const FColor& Color, const FVector& DoorSize, const FIntVector& DoorCell, const EDoorDirection& DoorRot, const FTransform& Transform, bool includeOffset, bool isConnected)
{
	FQuat DoorRotation = Transform.GetRotation() * ToQuaternion(!DoorRot ? EDoorDirection::North : DoorRot);
	FVector DoorPosition = Transform.TransformPosition(GetRealDoorPosition(DoorCell, DoorRot, includeOffset) + FVector(0, 0, DoorSize.Z * 0.5f));

	// Door frame
	DrawDebugBox(World, DoorPosition, DoorSize * 0.5f, DoorRotation, Color);

	if (isConnected)
	{
		// Arrow (there is a room on the other side OR in the editor preview)
		DrawDebugDirectionalArrow(World, DoorPosition, DoorPosition + DoorRotation * FVector(300, 0, 0), 300, Color);
	}
	else
	{
		// Cross (there is no room on the other side AND NOT in the editor preview)
		FVector HalfSize = DoorRotation * FVector(0, DoorSize.Y, DoorSize.Z) * 0.5f;
		FVector HalfSizeConjugate = DoorRotation * FVector(0, DoorSize.Y, -DoorSize.Z) * 0.5f;
		DrawDebugLine(World, DoorPosition - HalfSize, DoorPosition + HalfSize, Color);
		DrawDebugLine(World, DoorPosition - HalfSizeConjugate, DoorPosition + HalfSizeConjugate, Color);
	}
}

// ############ FBoxMinAndMax ##############

FBoxMinAndMax::FBoxMinAndMax(const FIntVector& A, const FIntVector& B)
{
	Min = IntVector::Min(A, B);
	Max = IntVector::Max(A, B);
}

FIntVector FBoxMinAndMax::GetSize() const
{
	return Max - Min;
}

bool FBoxMinAndMax::Overlap(const FBoxMinAndMax& A, const FBoxMinAndMax& B)
{
	return (A.Max.X > B.Min.X && A.Min.X < B.Max.X)
		&& (A.Max.Y > B.Min.Y && A.Min.Y < B.Max.Y)
		&& (A.Max.Z > B.Min.Z && A.Min.Z < B.Max.Z);
}

FBoxMinAndMax& FBoxMinAndMax::operator+=(const FIntVector& X)
{
	Min += X;
	Max += X;
	return *this;
}

FBoxMinAndMax& FBoxMinAndMax::operator-=(const FIntVector& X)
{
	Min -= X;
	Max -= X;
	return *this;
}

FBoxMinAndMax FBoxMinAndMax::operator+(const FIntVector& X) const
{
	FBoxMinAndMax NewBox(*this);
	NewBox += X;
	return NewBox;
}

FBoxMinAndMax FBoxMinAndMax::operator-(const FIntVector& X) const
{
	FBoxMinAndMax NewBox(*this);
	NewBox -= X;
	return NewBox;
}

FBoxMinAndMax Rotate(const FBoxMinAndMax& Box, const EDoorDirection& Rot)
{
	FIntVector Compensation = FIntVector::ZeroValue;
	switch (Rot)
	{
	case EDoorDirection::East:
		Compensation.X = 1;
		break;
	case EDoorDirection::West:
		Compensation.Y = 1;
		break;
	case EDoorDirection::South:
		Compensation.X = 1;
		Compensation.Y = 1;
		break;
	default:
		break;
	}
	return FBoxMinAndMax(Rotate(Box.Min, Rot) + Compensation, Rotate(Box.Max, Rot) + Compensation);
}
