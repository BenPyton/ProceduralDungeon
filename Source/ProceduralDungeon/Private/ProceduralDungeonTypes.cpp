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

FVector Rotate(const FVector& Pos, const EDoorDirection& Rot)
{
	FVector NewPos = Pos;
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

FDoorDef FDoorDef::GetOpposite() const
{
	FDoorDef OppositeDoor;
	OppositeDoor.Position = Position + ToIntVector(Direction);
	OppositeDoor.Direction = ~Direction;
	return OppositeDoor;
}

FBoxCenterAndExtent FDoorDef::GetBounds(bool bIncludeOffset) const
{
	const FVector RotatedDoorSize = Rotate(GetDoorSize(), (!Direction) ? EDoorDirection::North : Direction).GetAbs();
	const FVector WorldPosition = GetRealDoorPosition(Position, Direction, bIncludeOffset) + FVector(0, 0, RotatedDoorSize.Z * 0.5f);
	return FBoxCenterAndExtent(WorldPosition, 0.5f * RotatedDoorSize);
}

FVector FDoorDef::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool bIncludeOffset)
{
	const FVector CellPosition = FVector(DoorCell);
	const FVector DirectionOffset = !DoorRot ? FVector::ZeroVector : (0.5f * ToVector(DoorRot));
	const FVector HeightOffset = bIncludeOffset ? FVector(0, 0, Dungeon::DoorOffset()) : FVector::ZeroVector;
	return Dungeon::RoomUnit() * (CellPosition + DirectionOffset + HeightOffset);
}

#if !UE_BUILD_SHIPPING
void FDoorDef::DrawDebug(const UWorld* World, const FColor& Color, const FDoorDef& DoorDef, const FTransform& Transform, bool bIncludeOffset, bool isConnected)
{
	DrawDebug(World, Color, DoorDef.GetDoorSize(), DoorDef.Position, DoorDef.Direction, Transform, bIncludeOffset, isConnected);

	// Door debug draw using its bounds
	//FBoxCenterAndExtent DoorBounds = DoorDef.GetBounds(bIncludeOffset);
	//DrawDebugBox(World, Transform.TransformPosition(DoorBounds.Center), DoorBounds.Extent, Transform.GetRotation(), FColor::Cyan);
}

void FDoorDef::DrawDebug(const UWorld* World, const FColor& Color, const FVector& DoorSize, const FIntVector& DoorCell, const EDoorDirection& DoorRot, const FTransform& Transform, bool bIncludeOffset, bool isConnected)
{
#if ENABLE_DRAW_DEBUG
	// @TODO: Use FDoorDef::GetBounds here? (should mabye remove this overload and use exclusively the one with FDoorDef?)
	FQuat DoorRotation = Transform.GetRotation() * ToQuaternion(!DoorRot ? EDoorDirection::North : DoorRot);
	FVector DoorPosition = Transform.TransformPosition(GetRealDoorPosition(DoorCell, DoorRot, bIncludeOffset) + FVector(0, 0, DoorSize.Z * 0.5f));

	// Door frame
	DrawDebugBox(World, DoorPosition, DoorSize * 0.5f, DoorRotation, Color);

	if (isConnected)
	{
		// Arrow (there is a room on the other side OR in the editor preview)
		FVector ArrowVector = (Dungeon::FlipDoorArrow() ? -1.0f : 1.0f) * FVector(Dungeon::DoorArrowLength(), 0.0f, 0.0f);
		DrawDebugDirectionalArrow(World, DoorPosition, DoorPosition + DoorRotation * ArrowVector, Dungeon::DoorArrowHeadSize(), Color);
	}
	else
	{
		// Cross (there is no room on the other side AND NOT in the editor preview)
		FVector HalfSize = DoorRotation * FVector(0, DoorSize.Y, DoorSize.Z) * 0.5f;
		FVector HalfSizeConjugate = DoorRotation * FVector(0, DoorSize.Y, -DoorSize.Z) * 0.5f;
		DrawDebugLine(World, DoorPosition - HalfSize, DoorPosition + HalfSize, Color);
		DrawDebugLine(World, DoorPosition - HalfSizeConjugate, DoorPosition + HalfSizeConjugate, Color);
	}
#endif // ENABLE_DRAW_DEBUG
}
#endif // !UE_BUILD_SHIPPING

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

FBoxCenterAndExtent FBoxMinAndMax::ToCenterAndExtent() const
{
	FVector Center = 0.5f * FVector(Min + Max - FIntVector(1, 1, 0));
	FVector Extent = 0.5f * FVector(Max - Min);
	return FBoxCenterAndExtent(Center, Extent.GetAbs());
}

bool FBoxMinAndMax::IsInside(const FIntVector& Cell) const
{
	return (Cell.X >= Min.X) && (Cell.X < Max.X)
		&& (Cell.Y >= Min.Y) && (Cell.Y < Max.Y)
		&& (Cell.Z >= Min.Z) && (Cell.Z < Max.Z);
}

bool FBoxMinAndMax::IsInside(const FBoxMinAndMax& Other) const
{
	return (Other.Min.X >= Min.X) && (Other.Max.X <= Max.X)
		&& (Other.Min.Y >= Min.Y) && (Other.Max.Y <= Max.Y)
		&& (Other.Min.Z >= Min.Z) && (Other.Max.Z <= Max.Z);
}

void FBoxMinAndMax::Rotate(const EDoorDirection& Rot)
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
	const FIntVector A = ::Rotate(Min, Rot) + Compensation;
	const FIntVector B = ::Rotate(Max, Rot) + Compensation;
	Min = IntVector::Min(A, B);
	Max = IntVector::Max(A, B);
}

FString FBoxMinAndMax::ToString() const
{
	return FString::Printf(TEXT("[(%d, %d, %d), (%d, %d, %d)]"), Min.X, Min.Y, Min.Z, Max.X, Max.Y, Max.Z);
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

bool FBoxMinAndMax::operator==(const FBoxMinAndMax& Other) const
{
	return (Min == Other.Min) && (Max == Other.Max);
}

bool FBoxMinAndMax::operator!=(const FBoxMinAndMax& Other) const
{
	return !FBoxMinAndMax::operator==(Other);
}

FBoxMinAndMax Rotate(const FBoxMinAndMax& Box, const EDoorDirection& Rot)
{
	FBoxMinAndMax NewBox(Box);
	NewBox.Rotate(Rot);
	return NewBox;
}
