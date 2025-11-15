// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
	return FRotator(0.0f, ToAngle(Direction), 0.0f).Quaternion();
}

float ToAngle(const EDoorDirection& Direction)
{
	check(!!Direction);
	return 90.0f * static_cast<uint8>(Direction);
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

FIntVector Transform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return Rotate(Pos, Rotation) + Translation;
}

FIntVector InverseTransform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return Rotate(Pos - Translation, -Rotation);
}

EDoorDirection Transform(const EDoorDirection& Direction, const EDoorDirection& Rotation)
{
	return Direction + Rotation;
}

EDoorDirection InverseTransform(const EDoorDirection& Direction, const EDoorDirection& Rotation)
{
	return Direction - Rotation;
}

// ############ FDoorDef ##############

const FDoorDef FDoorDef::Invalid(FIntVector::ZeroValue, EDoorDirection::NbDirection);

FDoorDef::FDoorDef(const FIntVector& InPosition, EDoorDirection InDirection, UDoorType* InType)
{
	Position = InPosition;
	Direction = InDirection;
	Type = InType;
}

bool FDoorDef::IsValid() const
{
	return Direction != EDoorDirection::NbDirection;
}

bool FDoorDef::operator==(const FDoorDef& Other) const
{
	return Position == Other.Position && Direction == Other.Direction;
}

bool FDoorDef::AreCompatible(const FDoorDef& A, const FDoorDef& B)
{
	return UDoorType::AreCompatible(A.Type, B.Type);
}

FVector FDoorDef::GetDoorSize() const
{
	return UDoorType::GetSize(Type);
}

float FDoorDef::GetDoorOffset() const
{
	return UDoorType::GetOffset(Type);
}

FColor FDoorDef::GetDoorColor() const
{
	return UDoorType::GetColor(Type);
}

FString FDoorDef::GetTypeName() const
{
	return ::IsValid(Type) ? Type->GetName() : TEXT("Default");
}

FString FDoorDef::ToString() const
{
	FText DirectionName;
	UEnum::GetDisplayValueAsText(Direction, DirectionName);
	return FString::Printf(TEXT("(%d,%d,%d) [%s]"), Position.X, Position.Y, Position.Z, *DirectionName.ToString());
}

FDoorDef FDoorDef::GetOpposite() const
{
	FDoorDef OppositeDoor(*this);
	OppositeDoor.Position = Position + ToIntVector(Direction);
	OppositeDoor.Direction = ~Direction;
	return OppositeDoor;
}

FBoxCenterAndExtent FDoorDef::GetBounds(const FVector RoomUnit, bool bIncludeOffset) const
{
	const FVector RotatedDoorSize = Rotate(GetDoorSize(), (!Direction) ? EDoorDirection::North : Direction).GetAbs();
	const FVector WorldPosition = GetRealDoorPosition(*this, RoomUnit, bIncludeOffset) + FVector(0, 0, RotatedDoorSize.Z * 0.5f);
	return FBoxCenterAndExtent(WorldPosition, 0.5f * RotatedDoorSize);
}

FVector FDoorDef::GetRealDoorPosition(const FDoorDef& DoorDef, const FVector RoomUnit, bool bIncludeOffset)
{
	return GetRealDoorPosition(DoorDef.Position, DoorDef.Direction, RoomUnit, bIncludeOffset ? DoorDef.GetDoorOffset() : 0.0f);
}

FVector FDoorDef::GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, const FVector RoomUnit, float DoorOffset)
{
	const FVector CellPosition = FVector(DoorCell);
	const FVector DirectionOffset = !DoorRot ? FVector::ZeroVector : (0.5f * ToVector(DoorRot));
	const FVector HeightOffset = FVector(0, 0, DoorOffset);
	return RoomUnit * (CellPosition + DirectionOffset + HeightOffset);
}

FQuat FDoorDef::GetRealDoorRotation(const FDoorDef& DoorDef, bool bFlipped)
{
	return FRotator(0, 90 * static_cast<uint8>(DoorDef.Direction) + bFlipped * 180, 0).Quaternion();
}

FDoorDef FDoorDef::Transform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation)
{
	FDoorDef NewDoor = DoorDef;
	NewDoor.Position = ::Transform(DoorDef.Position, Translation, Rotation);
	NewDoor.Direction = ::Transform(DoorDef.Direction, Rotation);
	return NewDoor;
}

FDoorDef FDoorDef::InverseTransform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation)
{
	FDoorDef NewDoor = DoorDef;
	NewDoor.Position = ::InverseTransform(DoorDef.Position, Translation, Rotation);
	NewDoor.Direction = ::InverseTransform(DoorDef.Direction, Rotation);
	return NewDoor;
}

#if !UE_BUILD_SHIPPING
void FDoorDef::DrawDebug(const UWorld* World, const FDoorDef& DoorDef, const FVector RoomUnit, const FTransform& Transform, bool bIncludeOffset, bool bIsConnected)
{
	DrawDebug(World, DoorDef.GetDoorColor(), DoorDef.GetDoorSize(), RoomUnit, DoorDef.Position, DoorDef.Direction, Transform, bIncludeOffset ? DoorDef.GetDoorOffset() : 0.0f, bIsConnected);

	// Door debug draw using its bounds
	//FBoxCenterAndExtent DoorBounds = DoorDef.GetBounds(bIncludeOffset);
	//DrawDebugBox(World, Transform.TransformPosition(DoorBounds.Center), DoorBounds.Extent, Transform.GetRotation(), FColor::Cyan);
}

void FDoorDef::DrawDebug(const UWorld* World, const FColor& Color, const FVector& DoorSize, const FVector RoomUnit, const FIntVector& DoorCell, const EDoorDirection& DoorRot, const FTransform& Transform, float DoorOffset, bool bIsConnected)
{
	#if ENABLE_DRAW_DEBUG
	// @TODO: Use FDoorDef::GetBounds here? (should mabye remove this overload and use exclusively the one with FDoorDef?)
	FQuat DoorRotation = Transform.GetRotation() * ToQuaternion(!DoorRot ? EDoorDirection::North : DoorRot);
	FVector DoorPosition = Transform.TransformPosition(GetRealDoorPosition(DoorCell, DoorRot, RoomUnit, DoorOffset) + FVector(0, 0, DoorSize.Z * 0.5f));

	// Door frame
	DrawDebugBox(World, DoorPosition, DoorSize * 0.5f, DoorRotation, Color);

	if (bIsConnected)
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

void FBoxMinAndMax::Extend(const FBoxMinAndMax& Other)
{
	if (Min != Max)
	{
		Min = IntVector::Min(Min, Other.Min);
		Max = IntVector::Max(Max, Other.Max);
	}
	else
	{
		Min = Other.Min;
		Max = Other.Max;
	}
}

FString FBoxMinAndMax::ToString() const
{
	return FString::Printf(TEXT("[(%d, %d, %d), (%d, %d, %d)]"), Min.X, Min.Y, Min.Z, Max.X, Max.Y, Max.Z);
}

FIntVector FBoxMinAndMax::GetClosestPoint(const FIntVector& Point) const
{
	return IntVector::Min(Max, IntVector::Max(Min, Point));
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

FRoomCandidate FRoomCandidate::Invalid = FRoomCandidate();
