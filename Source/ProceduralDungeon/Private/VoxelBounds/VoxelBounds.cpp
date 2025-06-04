// Copyright Benoit Pelletier 2025 All Rights Reserved.
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

TArray<FVoxelBoundsConnection>& FVoxelBounds::AddCell(FIntVector Cell)
{
	auto& Connections = Cells.Add(Cell);
	Connections.SetNum(static_cast<uint8>(EDirection::NbDirection));
	Bounds.Extend(FBoxMinAndMax(Cell, Cell + FIntVector(1)));
	return Connections;
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

bool FVoxelBounds::GetCompatibilityScore(const FVoxelBounds& Other, int32& Score, const FScoreCallback& CustomScore) const
{
	// Each cell add 1 to the score, so the bigger volume the higher score.
	Score = Cells.Num();

	bool bAreOverlapping = FBoxMinAndMax::Overlap(Bounds, Other.Bounds);

	// @TODO: for now, treating a coincident face as overlapping
	// There is room for further optimizations here later
	bAreOverlapping |= Bounds.Min.X == Other.Bounds.Max.X;
	bAreOverlapping |= Bounds.Max.X == Other.Bounds.Min.X;
	bAreOverlapping |= Bounds.Min.Y == Other.Bounds.Max.Y;
	bAreOverlapping |= Bounds.Max.Y == Other.Bounds.Min.Y;
	bAreOverlapping |= Bounds.Min.Z == Other.Bounds.Max.Z;
	bAreOverlapping |= Bounds.Max.Z == Other.Bounds.Min.Z;

	// When not overlapping, the score is equal to the number of cell
	// and it does always fit outside too.
	if (!bAreOverlapping)
	{
		return true;
	}

	for (const auto& Cell : Cells)
	{
		// When a cell is defined in both bounds, it does not fit outside
		if (Other.Cells.Contains(Cell.Key))
		{
			Score = -1;
			return false;
		}

		// Case when this cell is not defined in the other bounds
		// We set a score depending on the connection compatibility
		// @TODO: top and bottom are not yet relevant, but will be when doors on top/bottom will be implemented
		for (uint8 i = 0; i < static_cast<uint8>(EDoorDirection::NbDirection); ++i)
		{
			// Get Neighbor cell
			const FIntVector Neighbor = Cell.Key + Directions[i];
			auto* NeighConns = Other.Cells.Find(Neighbor);
			if (nullptr == NeighConns)
				continue;

			const auto& Connection = Cell.Value[i];
			const auto& OtherConnection = (*NeighConns)[static_cast<uint8>(Opposite(static_cast<EDirection>(i)))];

			if (CustomScore.IsBound())
			{
				if (!CustomScore.Execute(Connection, OtherConnection, Score))
					return false;
			}
			else
			{
				Score += FVoxelBoundsConnection::GetCompatibilityScore(Connection, OtherConnection);
			}
		}
	}

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
	return Bounds + (FIntVector::ZeroValue - Offset);
}

void FVoxelBounds::operator+=(const FVoxelBounds& Other)
{
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
