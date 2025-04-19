/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "ProceduralDungeonTypes.h"
#include "ProceduralDungeonUtils.h"
#include "VoxelBounds.generated.h"

class UDoorType;

// Base class for the different connection types.
struct FVoxelBoundsConnection
{
public:
	enum class EType
	{
		None,
		Wall,
		Door
	};

	FVoxelBoundsConnection() = default;
	FVoxelBoundsConnection(EType InType) : Type(InType) {}
	FVoxelBoundsConnection(UDoorType* InDoorType) : Type(EType::Door), DoorType(InDoorType) {}

	EType Type {EType::None};

	// Used when `Type` is `Door`
	// Same door types give a high score whereas different door types give a low score.
	UDoorType* DoorType {nullptr};

	bool IsValid() const { return EType::None != Type; }

	bool operator==(const FVoxelBoundsConnection& Other) const;
	bool operator!=(const FVoxelBoundsConnection& Other) const { return !(*this == Other); }

	static int32 GetCompatibilityScore(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B);
};

USTRUCT()
struct FVoxelBounds
{
	GENERATED_BODY()

public:
	enum class EDirection : uint8
	{
		North = 0, // { 1,  0,  0}
		East,	   // { 0,  1,  0}
		South,	   // {-1,  0,  0}
		West,	   // { 0, -1,  0}
		Up,		   // { 0,  0,  1}
		Down,	   // { 0,  0, -1}
		NbDirection
	};

	// All directions in the 3D space.
	static const FIntVector Directions[(uint8)EDirection::NbDirection];
	static EDirection Opposite(EDirection Direction);

	TArray<FVoxelBoundsConnection>& AddCell(FIntVector Cell);

	const FVoxelBoundsConnection* GetCellConnection(FIntVector Cell, EDirection Direction) const;
	bool SetCellConnection(FIntVector Cell, EDirection Direction, const FVoxelBoundsConnection& Connection);

	const FBoxMinAndMax& GetBounds() const { return Bounds; }
	bool IsValid() const { return Cells.Num() > 0; }

	// Checks how well the bounds fit together.
	// Returns true if the bounds fit together and sets the score.
	bool GetCompatibilityScore(const FVoxelBounds& Other, int32& Score) const;

	// Operators to offset the bounds
	void operator+=(const FIntVector& Offset);
	void operator-=(const FIntVector& Offset);
	friend FVoxelBounds operator+(const FVoxelBounds& Bounds, const FIntVector& Offset);
	friend FVoxelBounds operator-(const FVoxelBounds& Bounds, const FIntVector& Offset);

	// Operators to concatenate bounds cells
	void operator+=(const FVoxelBounds& Other);
	friend FVoxelBounds operator+(const FVoxelBounds& A, const FVoxelBounds& B);

	// Operators to remove bounds cells
	void operator-=(const FVoxelBounds& Other);
	friend FVoxelBounds operator-(const FVoxelBounds& A, const FVoxelBounds& B);

	// Operators to compare bounds
	bool operator==(const FVoxelBounds& Other) const;
	bool operator!=(const FVoxelBounds& Other) const { return !(*this == Other); }

	static bool Overlap(const FVoxelBounds& A, const FVoxelBounds& B);
	friend FVoxelBounds Rotate(const FVoxelBounds& Bounds, const EDoorDirection& Rot);

private:
	TMap<FIntVector, TArray<FVoxelBoundsConnection>> Cells;
	FBoxMinAndMax Bounds;
};
