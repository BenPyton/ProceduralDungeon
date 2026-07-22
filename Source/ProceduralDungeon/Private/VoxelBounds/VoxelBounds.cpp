// Copyright Benoit Pelletier 2025 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "VoxelBounds/VoxelBounds.h"

const FIntVector FVoxelBounds::Directions[] = {
	{1, 0, 0},	// North
	{0, 1, 0},	// East
	{-1, 0, 0}, // South
	{0, -1, 0}, // West
	{0, 0, 1},	// Up
	{0, 0, -1}	// Down
};

bool FVoxelBoundsConnection::operator==(const FVoxelBoundsConnection& Other) const
{
	if (Type != Other.Type)
		return false;
	if (EVoxelBoundsConnectionType::Door == Type)
		return DoorType == Other.DoorType;
	return true;
}

int32 FVoxelBoundsConnection::GetCompatibilityScore(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B)
{
	// No connection is always compatible with any other (that measn it's inside the bounds)
	if (A.Type == EVoxelBoundsConnectionType::None || B.Type == EVoxelBoundsConnectionType::None)
		return 0;

	// When types are mismatching, it's not compatible
	if (A.Type != B.Type)
	{
		// Penalty when a door is not aligned with another door
		if (EVoxelBoundsConnectionType::Door == A.Type || EVoxelBoundsConnectionType::Door == B.Type)
			return -10;
		return 0;
	}

	if (EVoxelBoundsConnectionType::Door == A.Type)
	{
		// High score when doors are aligned and matching together
		if (A.DoorType == B.DoorType)
			return 10;

		// Penalty when doors are aligned but not matching together
		return -10;
	}

	return 0;
}

FVoxelBounds::EDirection FVoxelBounds::Rotate(EDirection Direction, EDoorDirection Rotation)
{
	// Rotate only NSEW directions, leave untouched UP and DOWN
	return (static_cast<uint8>(Direction) < static_cast<uint8>(EDoorDirection::NbDirection))
		? static_cast<EDirection>(static_cast<EDoorDirection>(Direction) + Rotation)
		: Direction ;
}

FVoxelBounds::EDirection FVoxelBounds::Opposite(EDirection Direction)
{
	static const EDirection OppositeDirections[] = {
		EDirection::South,
		EDirection::West,
		EDirection::North,
		EDirection::East,
		EDirection::Down,
		EDirection::Up
	};

	const uint8 Index = static_cast<uint8>(Direction);
	if (Index < static_cast<uint8>(EDirection::NbDirection))
	{
		return OppositeDirections[Index];
	}

	return EDirection::NbDirection;
}

FVoxelBounds::FCell& FVoxelBounds::AddCell(FIntVector Cell)
{
	auto& NewCell = Cells.Add(Cell);
	Bounds.Extend(FBoxMinAndMax(Cell, Cell + FIntVector(1)));
	return NewCell;
}

void FVoxelBounds::AddBox(const FBoxMinAndMax& Box)
{
	Bounds.Extend(Box);
	Cells.Reserve(Cells.Num() + Box.GetSize().X * Box.GetSize().Y * Box.GetSize().Z);

	for (int32 X = Box.GetMin().X; X < Box.GetMax().X; ++X)
	{
		for (int32 Y = Box.GetMin().Y; Y < Box.GetMax().Y; ++Y)
		{
			for (int32 Z = Box.GetMin().Z; Z < Box.GetMax().Z; ++Z)
			{
				Cells.Add(FIntVector(X, Y, Z));
			}
		}
	}
}

const FVoxelBoundsConnection* FVoxelBounds::GetCellConnection(FIntVector Cell, EDirection Direction) const
{
	auto* CellConnections = Cells.Find(Cell);
	if (nullptr == CellConnections)
		return nullptr;
	return &(*CellConnections)[static_cast<uint8>(Direction)];
}

bool FVoxelBounds::SetCellConnection(FIntVector Cell, EDirection Direction, const FVoxelBoundsConnection& Connection)
{
	auto* CellConnections = Cells.Find(Cell);
	if (nullptr == CellConnections)
		return false;

	(*CellConnections)[static_cast<uint8>(Direction)] = Connection;
	return true;
}

void FVoxelBounds::ResetToWalls()
{
	static const FVoxelBoundsConnection NoneConnection(EVoxelBoundsConnectionType::None);
	static const FVoxelBoundsConnection WallConnection(EVoxelBoundsConnectionType::Wall);

	for (auto& Cell : Cells)
	{
		for (uint8 i = 0; i < static_cast<uint8>(EDirection::NbDirection); ++i)
		{
			const FIntVector OtherCell = Cell.Key + Directions[i];
			const auto* FoundOtherCell = Cells.Find(OtherCell);
			Cell.Value[i] = (FoundOtherCell) ? NoneConnection : WallConnection;
		}
	}
}

void FVoxelBounds::FlagBoundaryCells()
{
	for (auto& Pair : Cells)
	{
		Pair.Value.bBoundary = false;
		for (uint8 i = 0; i < static_cast<uint8>(EDoorDirection::NbDirection); ++i)
		{
			if (!Cells.Contains(Pair.Key + Directions[i]))
			{
				Pair.Value.bBoundary = true;
				break;
			}
		}
	}
}

bool FVoxelBounds::GetCompatibilityScore(const FVoxelBounds& Other, int32& Score, const FScoreFunction& ScoreFunc) const
{
	return GetCompatibilityScore(Other, FRoomTransform::Identity, Score, ScoreFunc);
}

bool FVoxelBounds::GetCompatibilityScore(const FVoxelBounds& Other, const FRoomTransform& Transform, int32& Score, const FScoreFunction& ScoreFunc) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::GetCompatibilityScore);
	// Each cell add 1 to the score, so the bigger volume the higher score.
	Score = Cells.Num();

	checkf(ScoreFunc, TEXT("ScoreFunc must be callable!"));

	const FBoxMinAndMax TransformedBounds = ::Rotate(Bounds, Transform.Rotation) + Transform.Translation;
	bool bAreOverlapping = FBoxMinAndMax::Overlap(TransformedBounds, Other.Bounds);

	// @TODO: for now, treating a coincident face as overlapping
	// There is room for further optimizations here later
	bAreOverlapping |= TransformedBounds.GetMin().X == Other.Bounds.GetMax().X;
	bAreOverlapping |= TransformedBounds.GetMax().X == Other.Bounds.GetMin().X;
	bAreOverlapping |= TransformedBounds.GetMin().Y == Other.Bounds.GetMax().Y;
	bAreOverlapping |= TransformedBounds.GetMax().Y == Other.Bounds.GetMin().Y;
	bAreOverlapping |= TransformedBounds.GetMin().Z == Other.Bounds.GetMax().Z;
	bAreOverlapping |= TransformedBounds.GetMax().Z == Other.Bounds.GetMin().Z;

	// When not overlapping, the score is equal to the number of cell
	// and it does always fit outside too.
	if (!bAreOverlapping)
	{
		return true;
	}

	for (const auto& Pair : Cells)
	{
		const FIntVector Cell = Transform.Transform(Pair.Key);

		// When a cell is defined in both bounds, it does not fit outside
		if (Other.Cells.Contains(Cell))
		{
			Score = -1;
			return false;
		}

		if (!Pair.Value.IsBoundary())
			continue;

		// Case when this cell is not defined in the other bounds
		// We set a score depending on the connection compatibility
		// @TODO: top and bottom are not yet relevant, but will be when doors on top/bottom will be implemented
		for (uint8 i = 0; i < static_cast<uint8>(EDoorDirection::NbDirection); ++i)
		{
			// Get Neighbor cell
			const EDirection RotatedDir = Rotate(static_cast<EDirection>(i), Transform.Rotation);
			const FIntVector Neighbor = Cell + Directions[static_cast<uint8>(RotatedDir)];
			auto* NeighConns = Other.Cells.Find(Neighbor);
			if (nullptr == NeighConns)
				continue;

			const auto& Connection = Pair.Value[i];
			const auto& OtherConnection = (*NeighConns)[static_cast<uint8>(Opposite(RotatedDir))];

			if (!ScoreFunc(Connection, OtherConnection, Score))
				return false;
		}
	}

	return true;
}

bool FVoxelBounds::DefaultScoreFunc(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B, int32& OutScore)
{
	OutScore += FVoxelBoundsConnection::GetCompatibilityScore(A, B);
	return true;
}

void FVoxelBounds::operator+=(const FIntVector& Offset)
{
	*this = *this + Offset;
}

void FVoxelBounds::operator-=(const FIntVector& Offset)
{
	*this = *this - Offset;
}

FVoxelBounds operator+(const FVoxelBounds& Bounds, const FIntVector& Offset)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::operator+(FIntVector));
	FVoxelBounds NewBounds;
	for (const auto& Cell : Bounds.Cells)
	{
		NewBounds.Cells.Add(Cell.Key + Offset, Cell.Value);
	}
	NewBounds.Bounds = Bounds.Bounds + Offset;
	return NewBounds;
}

FVoxelBounds operator-(const FVoxelBounds& Bounds, const FIntVector& Offset)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::operator-(FIntVector));
	return Bounds + (FIntVector::ZeroValue - Offset);
}

void FVoxelBounds::operator+=(const FVoxelBounds& Other)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::operator+=(FVoxelBounds));
	for (const auto& Cell : Other.Cells)
	{
		// Ignore incoming cells that are already defined
		// @TODO: how to manage different connections?
		if (Cells.Contains(Cell.Key))
			continue;

		auto& Connections = AddCell(Cell.Key);
		for (uint8 i = 0; i < static_cast<uint8>(EDirection::NbDirection); ++i)
		{
			// Get neighbor cell
			const FIntVector Neighbor = Cell.Key + Directions[i];
			if (auto* NeighConns = Cells.Find(Neighbor))
			{
				// If neighbor is defined, we clear the neigbor's connection
				// Also, we don't copy the connection of other bounds
				const uint8 OppositeDir = static_cast<uint8>(Opposite(static_cast<EDirection>(i)));
				(*NeighConns)[OppositeDir] = FVoxelBoundsConnection();
			}
			else
			{
				// Just copy connection if no neighbors
				Connections[i] = Cell.Value[i];
			}
		}
	}
}

void FVoxelBounds::operator-=(const FVoxelBounds& Other)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::operator-=(FVoxelBounds));
	for (const auto& Cell : Other.Cells)
	{
		if (!Cells.Remove(Cell.Key))
			continue;

		for (uint8 i = 0; i < static_cast<uint8>(EDirection::NbDirection); ++i)
		{
			// Get neighbor cell
			const FIntVector Neighbor = Cell.Key + Directions[i];
			if (auto* NeighConns = Cells.Find(Neighbor))
			{
				// If neighbor is defined, we copy this connection into it
				const uint8 OppositeDir = static_cast<uint8>(Opposite(static_cast<EDirection>(i)));
				(*NeighConns)[OppositeDir] = Cell.Value[i];
			}
		}
	}
}

FVoxelBounds operator+(const FVoxelBounds& A, const FVoxelBounds& B)
{
	FVoxelBounds Result = A;
	Result += B;
	return Result;
}

FVoxelBounds operator-(const FVoxelBounds& A, const FVoxelBounds& B)
{
	FVoxelBounds Result = A;
	Result -= B;
	return Result;
}

bool FVoxelBounds::operator==(const FVoxelBounds& Other) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::operator==(FVoxelBounds));
	if (Cells.Num() != Other.Cells.Num())
		return false;

	for (const auto& Cell : Cells)
	{
		const auto* OtherConnections = Other.Cells.Find(Cell.Key);
		if (!OtherConnections)
			return false;
		if (Cell.Value.Num() != OtherConnections->Num())
			return false;
		for (uint8 i = 0; i < Cell.Value.Num(); ++i)
		{
			if (Cell.Value[i] != (*OtherConnections)[i])
				return false;
		}
	}

	return true;
}

bool FVoxelBounds::Overlap(const FVoxelBounds& A, const FVoxelBounds& B)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::Overlap);
	if (!FBoxMinAndMax::Overlap(A.Bounds, B.Bounds))
		return false;

	// @TODO: Maybe it will be more performant to use a hierarchical partitioning
	// especially when using really small RoomUnits (like (1,1,1))
	for (const auto& Cell : A.Cells)
	{
		if (B.Cells.Contains(Cell.Key))
			return true;
	}
	return false;
}

FVoxelBounds Rotate(const FVoxelBounds& Bounds, const EDoorDirection& Rot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FVoxelBounds::Rotate);
	FVoxelBounds NewBounds;

	for (const auto& Cell : Bounds.Cells)
	{
		const FIntVector NewCell = Rotate(Cell.Key, Rot);
		auto& NewConnections = NewBounds.AddCell(NewCell);
		// @TODO: will need to update that when doors on top/bottom will be implemented
		for (uint8 i = 0; i < static_cast<uint8>(EDoorDirection::NbDirection); ++i)
		{
			NewConnections[static_cast<uint8>(static_cast<EDoorDirection>(i) + Rot)] = Cell.Value[i];
		}
		// @TODO: Currently, no rotation are applied on top/bottom connections
		// but they will be when doors on top/bottom will be implemented
		NewConnections[static_cast<uint8>(FVoxelBounds::EDirection::Up)] = Cell.Value[static_cast<uint8>(FVoxelBounds::EDirection::Up)];
		NewConnections[static_cast<uint8>(FVoxelBounds::EDirection::Down)] = Cell.Value[static_cast<uint8>(FVoxelBounds::EDirection::Down)];
	}
	return NewBounds;
}
