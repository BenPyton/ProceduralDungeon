/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "ProceduralDungeonTypes.h"
#include "VoxelBounds/VoxelBounds.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxelBoundsTest, "ProceduralDungeon.Types.VoxelBounds", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

#define SET_CONNECTION(BOUNDS, CELL, DIR, TYPE) \
	BOUNDS.SetCellConnection(FIntVector CELL, FVoxelBounds::EDirection::DIR, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::TYPE));

bool FVoxelBoundsTest::RunTest(const FString& Parameters)
{
	// Extend Test
	{
		FVoxelBounds BoundsA;
		FVoxelBounds BoundsB;
		FVoxelBounds BoundsC;

		BoundsA.AddCell(FIntVector(0, -1, 0));
		BoundsA.AddCell(FIntVector(1, 1, 2));

		BoundsB.AddCell(FIntVector(1, 1, 1));

		TestEqual(TEXT("BoundsA == ({0, -1, 0}, {2, 2, 3})"), BoundsA.GetBounds(), FBoxMinAndMax({0, -1, 0}, {2, 2, 3}));
		TestEqual(TEXT("BoundsB == ({1, 1, 1}, {2, 2, 2})"), BoundsB.GetBounds(), FBoxMinAndMax({1, 1, 1}, {2, 2, 2}));
		TestEqual(TEXT("BoundsC == ({0, 0, 0}, {0, 0, 0})"), BoundsC.GetBounds(), FBoxMinAndMax());
	}

	// Comparison Test
	{
		FVoxelBounds BoundsA;
		FVoxelBounds BoundsB;
		FVoxelBounds BoundsC;
		FVoxelBounds BoundsD;
		FVoxelBounds BoundsE;

		BoundsA.AddCell(FIntVector(0, 0, 0));
		BoundsA.AddCell(FIntVector(1, 1, 1));
		SET_CONNECTION(BoundsA, (0, 0, 0), North, Wall);
		SET_CONNECTION(BoundsA, (1, 1, 1), South, Door);

		// Setting the cell connections in a different way is intentional here
		auto& CellA = BoundsB.AddCell(FIntVector(0, 0, 0));
		auto& CellB = BoundsB.AddCell(FIntVector(1, 1, 1));
		CellA[static_cast<uint8>(FVoxelBounds::EDirection::North)] = FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall);
		CellB[static_cast<uint8>(FVoxelBounds::EDirection::South)] = FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door);

		BoundsC.AddCell(FIntVector(0, 0, 0));
		BoundsC.AddCell(FIntVector(1, 1, 1));
		SET_CONNECTION(BoundsC, (0, 0, 0), North, Wall);

		TestEqual(TEXT("BoundsA == BoundsB"), BoundsA, BoundsB);
		TestNotEqual(TEXT("BoundsA != BoundsC"), BoundsA, BoundsC);
		TestTrue(TEXT("BoundsD is not valid"), !BoundsD.IsValid());
		TestNotEqual(TEXT("BoundsA != BoundsD"), BoundsA, BoundsD);
	}

	// Transform Test
	{
		FVoxelBounds Bounds;
		Bounds.AddCell(FIntVector(0, 0, 0));
		Bounds.AddCell(FIntVector(1, 1, 1));
		SET_CONNECTION(Bounds, (0, 0, 0), North, Wall);
		SET_CONNECTION(Bounds, (1, 1, 1), South, Door);

		// Offset by 0
		{
			TestEqual(TEXT("Bounds + (0,0,0) == OffsetBounds"), Bounds + FIntVector {0,0,0}, Bounds);
		}

		// Simple addition
		{
			FVoxelBounds OffsetBounds;
			OffsetBounds.AddCell(FIntVector(1, -1, 1));
			OffsetBounds.AddCell(FIntVector(2, 0, 2));
			SET_CONNECTION(OffsetBounds, (1, -1, 1), North, Wall);
			SET_CONNECTION(OffsetBounds, (2, 0, 2), South, Door);

			TestEqual(TEXT("Bounds + (1,-1,1) == OffsetBounds"), Bounds + FIntVector {1,-1,1}, OffsetBounds);
		}

		// Simple subtraction
		{
			FVoxelBounds OffsetBounds;
			OffsetBounds.AddCell(FIntVector(-1, 1, -1));
			OffsetBounds.AddCell(FIntVector(0, 2, 0));
			SET_CONNECTION(OffsetBounds, (-1, 1, -1), North, Wall);
			SET_CONNECTION(OffsetBounds, (0, 2, 0), South, Door);
			TestEqual(TEXT("Bounds - (1,-1,1) == OffsetBounds"), Bounds - FIntVector {1,-1,1}, OffsetBounds);
		}

		// North rotation
		{
			TestEqual(TEXT("Bounds.Rotate(North) == Bounds"), Rotate(Bounds, EDoorDirection::North), Bounds);
		}

		// East rotation
		{
			FVoxelBounds RotatedBounds;
			RotatedBounds.AddCell(FIntVector(0, 0, 0));
			RotatedBounds.AddCell(FIntVector(-1, 1, 1));
			SET_CONNECTION(RotatedBounds, (0, 0, 0), East, Wall);
			SET_CONNECTION(RotatedBounds, (-1, 1, 1), West, Door);
			FVoxelBounds NewBounds = Rotate(Bounds, EDoorDirection::East);
			TestEqual(TEXT("Bounds.Rotate(East) == RotatedBounds"), NewBounds, RotatedBounds);
		}

		// South rotation
		{
			FVoxelBounds RotatedBounds;
			RotatedBounds.AddCell(FIntVector(0, 0, 0));
			RotatedBounds.AddCell(FIntVector(-1, -1, 1));
			SET_CONNECTION(RotatedBounds, (0, 0, 0), South, Wall);
			SET_CONNECTION(RotatedBounds, (-1, -1, 1), North, Door);
			TestEqual(TEXT("Bounds.Rotate(South) == RotatedBounds"), Rotate(Bounds, EDoorDirection::South), RotatedBounds);
		}

		// West rotation
		{
			FVoxelBounds RotatedBounds;
			RotatedBounds.AddCell(FIntVector(0, 0, 0));
			RotatedBounds.AddCell(FIntVector(1, -1, 1));
			SET_CONNECTION(RotatedBounds, (0, 0, 0), West, Wall);
			SET_CONNECTION(RotatedBounds, (1, -1, 1), East, Door);
			FVoxelBounds NewBounds = Rotate(Bounds, EDoorDirection::West);
			TestEqual(TEXT("Bounds.Rotate(West) == RotatedBounds"), NewBounds, RotatedBounds);
		}
	}

	// Concatenation Test
	{
		FVoxelBounds BoundsA;
		BoundsA.AddCell(FIntVector(0, 0, 0));
		SET_CONNECTION(BoundsA, (0, 0, 0), North, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), East, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), South, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), West, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsA, (0, 0, 0), Down, Wall);

		FVoxelBounds BoundsB;
		BoundsB.AddCell(FIntVector(1, 0, 0));
		SET_CONNECTION(BoundsB, (1, 0, 0), North, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), East, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), South, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), West, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), Down, Wall);

		FVoxelBounds BoundsC = BoundsA + BoundsB;
		FVoxelBounds BoundsD = BoundsB + BoundsA;

		TestEqual(TEXT("BoundsC == BoundsD"), BoundsC, BoundsD);
		TestEqual(TEXT("BoundsC.GetBounds() == BoundsD.GetBounds()"), BoundsC.GetBounds(), BoundsD.GetBounds());

		FVoxelBounds ExpectedBounds;
		ExpectedBounds.AddCell(FIntVector(0, 0, 0));
		ExpectedBounds.AddCell(FIntVector(1, 0, 0));
		SET_CONNECTION(ExpectedBounds, (0, 0, 0), East, Door);
		SET_CONNECTION(ExpectedBounds, (0, 0, 0), South, Door);
		SET_CONNECTION(ExpectedBounds, (0, 0, 0), West, Door);
		SET_CONNECTION(ExpectedBounds, (0, 0, 0), Up, Wall);
		SET_CONNECTION(ExpectedBounds, (0, 0, 0), Down, Wall);

		SET_CONNECTION(ExpectedBounds, (1, 0, 0), North, Wall);
		SET_CONNECTION(ExpectedBounds, (1, 0, 0), East, Wall);
		SET_CONNECTION(ExpectedBounds, (1, 0, 0), West, Wall);
		SET_CONNECTION(ExpectedBounds, (1, 0, 0), Up, Wall);
		SET_CONNECTION(ExpectedBounds, (1, 0, 0), Down, Wall);

		TestEqual(TEXT("BoundsC == ExpectedBounds"), BoundsC, ExpectedBounds);
	}

	// Subtraction test
	{
		FVoxelBounds BoundsA;
		BoundsA.AddCell(FIntVector(0, 0, 0));
		SET_CONNECTION(BoundsA, (0, 0, 0), North, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), East, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), South, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), West, Door);
		SET_CONNECTION(BoundsA, (0, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsA, (0, 0, 0), Down, Wall);

		FVoxelBounds BoundsB;
		BoundsB.AddCell(FIntVector(1, 0, 0));
		SET_CONNECTION(BoundsB, (1, 0, 0), North, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), East, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), South, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), West, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsB, (1, 0, 0), Down, Wall);

		FVoxelBounds BoundsC;
		BoundsC.AddCell(FIntVector(0, 0, 0));
		BoundsC.AddCell(FIntVector(1, 0, 0));
		SET_CONNECTION(BoundsC, (0, 0, 0), East, Door);
		SET_CONNECTION(BoundsC, (0, 0, 0), South, Door);
		SET_CONNECTION(BoundsC, (0, 0, 0), West, Door);
		SET_CONNECTION(BoundsC, (0, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsC, (0, 0, 0), Down, Wall);

		SET_CONNECTION(BoundsC, (1, 0, 0), North, Wall);
		SET_CONNECTION(BoundsC, (1, 0, 0), East, Wall);
		SET_CONNECTION(BoundsC, (1, 0, 0), West, Wall);
		SET_CONNECTION(BoundsC, (1, 0, 0), Up, Wall);
		SET_CONNECTION(BoundsC, (1, 0, 0), Down, Wall);

		FVoxelBounds ExpectedBoundsCMinusA;
		ExpectedBoundsCMinusA.AddCell(FIntVector(1, 0, 0));
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), North, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), East, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), South, Door);
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), West, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), Up, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusA, (1, 0, 0), Down, Wall);

		FVoxelBounds ExpectedBoundsCMinusB;
		ExpectedBoundsCMinusB.AddCell(FIntVector(0, 0, 0));
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), North, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), East, Door);
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), South, Door);
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), West, Door);
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), Up, Wall);
		SET_CONNECTION(ExpectedBoundsCMinusB, (0, 0, 0), Down, Wall);

		FVoxelBounds BoundsCMinusA = BoundsC - BoundsA;
		FVoxelBounds BoundsCMinusB = BoundsC - BoundsB;

		TestEqual(TEXT("BoundsC - BoundsA == ExpectedBoundsCMinusA"), BoundsCMinusA, ExpectedBoundsCMinusA);
		TestEqual(TEXT("BoundsC - BoundsB == ExpectedBoundsCMinusB"), BoundsCMinusB, ExpectedBoundsCMinusB);

		// Non commutative
		FVoxelBounds BoundsAMinusC = BoundsA - BoundsC;
		FVoxelBounds BoundsBMinusC = BoundsB - BoundsC;

		TestFalse(TEXT("BoundsA - BoundsC is invalid"), BoundsAMinusC.IsValid());
		TestFalse(TEXT("BoundsB - BoundsC is invalid"), BoundsBMinusC.IsValid());
	}

	// DoesFitOutside Test
	{
		// Bounds of 2x2x2
		// +---+-o-+	+   +---+
		// |100 110|	    |111|
		// +-o-+   +	+   +-o-+
		//     |010|	
		// +   +---+	+   +   +
		FVoxelBounds DungeonBounds;
		DungeonBounds.AddCell(FIntVector(1, 0, 0));
		DungeonBounds.AddCell(FIntVector(0, 1, 0));
		DungeonBounds.AddCell(FIntVector(1, 1, 0));
		DungeonBounds.AddCell(FIntVector(1, 1, 1));

		SET_CONNECTION(DungeonBounds, (1, 0, 0), North, Wall);
		SET_CONNECTION(DungeonBounds, (1, 0, 0), South, Door);
		SET_CONNECTION(DungeonBounds, (1, 0, 0), West, Wall);
		SET_CONNECTION(DungeonBounds, (1, 0, 0), Up, Wall);
		SET_CONNECTION(DungeonBounds, (1, 0, 0), Down, Wall);

		SET_CONNECTION(DungeonBounds, (0, 1, 0), West, Wall);
		SET_CONNECTION(DungeonBounds, (0, 1, 0), East, Door);
		SET_CONNECTION(DungeonBounds, (0, 1, 0), South, Wall);
		SET_CONNECTION(DungeonBounds, (0, 1, 0), Up, Wall);
		SET_CONNECTION(DungeonBounds, (0, 1, 0), Down, Wall);

		SET_CONNECTION(DungeonBounds, (1, 1, 0), North, Door);
		SET_CONNECTION(DungeonBounds, (1, 1, 0), East, Wall);
		SET_CONNECTION(DungeonBounds, (1, 1, 0), Down, Wall);

		SET_CONNECTION(DungeonBounds, (1, 1, 1), North, Wall);
		SET_CONNECTION(DungeonBounds, (1, 1, 1), East, Wall);
		SET_CONNECTION(DungeonBounds, (1, 1, 1), South, Door);
		SET_CONNECTION(DungeonBounds, (1, 1, 1), West, Wall);
		SET_CONNECTION(DungeonBounds, (1, 1, 1), Up, Wall);

		// Bounds that fit in the available space
		{
			// Bounds that fits perfectly in the available space
			// +   +   +
			//			
			// +-o-+   +
			// |000|
			// +---+   +
			FVoxelBounds BoundsA;
			BoundsA.AddCell(FIntVector(0, 0, 0));
			SET_CONNECTION(BoundsA, (0, 0, 0), North, Door);
			SET_CONNECTION(BoundsA, (0, 0, 0), East, Wall);
			SET_CONNECTION(BoundsA, (0, 0, 0), South, Wall);
			SET_CONNECTION(BoundsA, (0, 0, 0), West, Wall);
			SET_CONNECTION(BoundsA, (0, 0, 0), Up, Wall);
			SET_CONNECTION(BoundsA, (0, 0, 0), Down, Wall);

			// Like BoundsA but with a door facing a wall
			// +   +   +
			//			
			// +-o-+   +
			// |000o
			// +---+   +
			FVoxelBounds BoundsB;
			BoundsB.AddCell(FIntVector(0, 0, 0));
			SET_CONNECTION(BoundsB, (0, 0, 0), North, Door);
			SET_CONNECTION(BoundsB, (0, 0, 0), East, Door);
			SET_CONNECTION(BoundsB, (0, 0, 0), South, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), West, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), Up, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), Down, Wall);

			// Bounds bigger that fits perfectly in the available space
			// +-o-+   +
			// |101|
			// +   +   +
			// |001o
			// +---+   +
			FVoxelBounds BoundsC;
			BoundsC.AddCell(FIntVector(0, 0, 1));
			BoundsC.AddCell(FIntVector(1, 0, 1));

			SET_CONNECTION(BoundsC, (0, 0, 1), East, Door);
			SET_CONNECTION(BoundsC, (0, 0, 1), South, Wall);
			SET_CONNECTION(BoundsC, (0, 0, 1), West, Wall);
			SET_CONNECTION(BoundsC, (0, 0, 1), Up, Wall);
			SET_CONNECTION(BoundsC, (0, 0, 1), Down, Wall);

			SET_CONNECTION(BoundsC, (1, 0, 1), North, Door);
			SET_CONNECTION(BoundsC, (1, 0, 1), West, Wall);
			SET_CONNECTION(BoundsC, (1, 0, 1), East, Wall);
			SET_CONNECTION(BoundsC, (1, 0, 1), Up, Wall);
			SET_CONNECTION(BoundsC, (1, 0, 1), Down, Wall);

			// Like BoundsC but with a door facing a wall
			// +---+   +
			// |101o
			// +   +   +
			// |001o
			// +---+   +
			FVoxelBounds BoundsD;
			BoundsD.AddCell(FIntVector(0, 0, 1));
			BoundsD.AddCell(FIntVector(1, 0, 1));

			SET_CONNECTION(BoundsD, (0, 0, 1), East, Door);
			SET_CONNECTION(BoundsD, (0, 0, 1), South, Wall);
			SET_CONNECTION(BoundsD, (0, 0, 1), West, Wall);
			SET_CONNECTION(BoundsD, (0, 0, 1), Up, Wall);
			SET_CONNECTION(BoundsD, (0, 0, 1), Down, Wall);

			SET_CONNECTION(BoundsD, (1, 0, 1), North, Wall);
			SET_CONNECTION(BoundsD, (1, 0, 1), West, Wall);
			SET_CONNECTION(BoundsD, (1, 0, 1), East, Door);
			SET_CONNECTION(BoundsD, (1, 0, 1), Up, Wall);
			SET_CONNECTION(BoundsD, (1, 0, 1), Down, Wall);

			// Bounds outside the dungeon bounds
			// +   +---+	+   +-o-+
			//     |210|	    |211|
			// +   +-o-+	+   +---+
			FVoxelBounds BoundsE;
			BoundsE.AddCell(FIntVector(2, 1, 0));
			BoundsE.AddCell(FIntVector(2, 1, 1));
			SET_CONNECTION(BoundsE, (2, 1, 0), North, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 0), East, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 0), South, Door);
			SET_CONNECTION(BoundsE, (2, 1, 0), West, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 0), Down, Wall);

			SET_CONNECTION(BoundsE, (2, 1, 1), North, Door);
			SET_CONNECTION(BoundsE, (2, 1, 1), East, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 1), South, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 1), West, Wall);
			SET_CONNECTION(BoundsE, (2, 1, 1), Up, Wall);

			// Like BoundsE but with a door facing a wall
			// +   +---+	+   +---+
			//     |210|	    |211|
			// +   +-o-+	+   +-o-+
			FVoxelBounds BoundsF;
			BoundsF.AddCell(FIntVector(2, 1, 0));
			BoundsF.AddCell(FIntVector(2, 1, 1));
			SET_CONNECTION(BoundsF, (2, 1, 0), North, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 0), East, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 0), South, Door);
			SET_CONNECTION(BoundsF, (2, 1, 0), West, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 0), Down, Wall);

			SET_CONNECTION(BoundsF, (2, 1, 1), North, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 1), East, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 1), South, Door);
			SET_CONNECTION(BoundsF, (2, 1, 1), West, Wall);
			SET_CONNECTION(BoundsF, (2, 1, 1), Up, Wall);

			// Bounds bigger with a connected door
			// +   +   +
			// 
			// +-o-+-o-+
			// |001 011|
			// +---+---+
			FVoxelBounds BoundsG;
			BoundsG.AddCell(FIntVector(0, 0, 1));
			BoundsG.AddCell(FIntVector(0, 1, 1));

			SET_CONNECTION(BoundsG, (0, 0, 1), North, Door);
			SET_CONNECTION(BoundsG, (0, 0, 1), South, Wall);
			SET_CONNECTION(BoundsG, (0, 0, 1), West, Wall);
			SET_CONNECTION(BoundsG, (0, 0, 1), Up, Wall);
			SET_CONNECTION(BoundsG, (0, 0, 1), Down, Wall);

			SET_CONNECTION(BoundsG, (0, 1, 1), North, Door);
			SET_CONNECTION(BoundsG, (0, 1, 1), South, Wall);
			SET_CONNECTION(BoundsG, (0, 1, 1), East, Wall);
			SET_CONNECTION(BoundsG, (0, 1, 1), Up, Wall);
			SET_CONNECTION(BoundsG, (0, 1, 1), Down, Wall);

			int32 _ScoreA, _ScoreB, _ScoreC, _ScoreD, _ScoreE, _ScoreF, _ScoreG;
			TestTrue(TEXT("BoundsA fits in DungeonBounds"), BoundsA.GetCompatibilityScore(DungeonBounds, _ScoreA));
			TestTrue(TEXT("BoundsB fits in DungeonBounds"), BoundsB.GetCompatibilityScore(DungeonBounds, _ScoreB));
			TestTrue(TEXT("BoundsC fits in DungeonBounds"), BoundsC.GetCompatibilityScore(DungeonBounds, _ScoreC));
			TestTrue(TEXT("BoundsD fits in DungeonBounds"), BoundsD.GetCompatibilityScore(DungeonBounds, _ScoreD));
			TestTrue(TEXT("BoundsE fits in DungeonBounds"), BoundsE.GetCompatibilityScore(DungeonBounds, _ScoreE));
			TestTrue(TEXT("BoundsF fits in DungeonBounds"), BoundsF.GetCompatibilityScore(DungeonBounds, _ScoreF));
			TestTrue(TEXT("BoundsG fits in DungeonBounds"), BoundsG.GetCompatibilityScore(DungeonBounds, _ScoreG));

			// Order of fitting: E(G) > A > C(F) > B > D
			TestTrue(TEXT("BoundsE fits better than BoundsA"), _ScoreE > _ScoreA);
			TestTrue(TEXT("BoundsA fits better than BoundsC"), _ScoreA > _ScoreC);
			TestTrue(TEXT("BoundsC fits better than BoundsB"), _ScoreC > _ScoreB);
			TestTrue(TEXT("BoundsB fits better than BoundsD"), _ScoreB > _ScoreD);
			TestTrue(TEXT("BoundsG fits equally as BoundsE"), _ScoreG == _ScoreE);
			TestTrue(TEXT("BoundsC fits equally as BoundsF"), _ScoreC == _ScoreF);
		}

		// Bounds that do not fit in the available space
		{
			// Bounds that does not fit in the available space
			// +---+   +
			// |100|
			// +-o-+   +
			//
			// +   +   +
			FVoxelBounds BoundsA;
			BoundsA.AddCell(FIntVector(1, 0, 0));
			SET_CONNECTION(BoundsA, (1, 0, 0), East, Wall);
			SET_CONNECTION(BoundsA, (1, 0, 0), North, Door);
			SET_CONNECTION(BoundsA, (1, 0, 0), West, Wall);
			SET_CONNECTION(BoundsA, (1, 0, 0), Up, Wall);
			SET_CONNECTION(BoundsA, (1, 0, 0), Down, Wall);

			int32 ScoreA;
			TestFalse(TEXT("BoundsA does not fits in DungeonBounds"), BoundsA.GetCompatibilityScore(DungeonBounds, ScoreA));

			// Bigger bounds that does not fit in the available space
			// +---+   +
			// |100|
			// +   +   +
			// |000|
			// +-o-+   +
			FVoxelBounds BoundsB;
			BoundsB.AddCell(FIntVector(0, 0, 0));
			BoundsB.AddCell(FIntVector(1, 0, 0));

			SET_CONNECTION(BoundsB, (0, 0, 0), North, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), East, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), South, Door);
			SET_CONNECTION(BoundsB, (0, 0, 0), West, Wall);
			SET_CONNECTION(BoundsB, (0, 0, 0), Up, Wall);

			SET_CONNECTION(BoundsB, (1, 0, 0), North, Wall);
			SET_CONNECTION(BoundsB, (1, 0, 0), West, Wall);
			SET_CONNECTION(BoundsB, (1, 0, 0), East, Wall);
			SET_CONNECTION(BoundsB, (1, 0, 0), Up, Wall);
			SET_CONNECTION(BoundsB, (1, 0, 0), Down, Wall);

			int32 ScoreB;
			TestFalse(TEXT("BoundsB does not fits in DungeonBounds"), BoundsB.GetCompatibilityScore(DungeonBounds, ScoreB));
		}
	}

	return true;
}

#undef SET_CONNECTION

#endif //WITH_DEV_AUTOMATION_TESTS
