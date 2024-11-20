/*
 * MIT License
 *
 * Copyright (c) 2023-2024 Benoit Pelletier
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

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "ProceduralDungeonTypes.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxMinAndMaxTest, "ProceduralDungeon.Types.BoxMinAndMax", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FBoxMinAndMaxTest::RunTest(const FString& Parameters)
{
	// Constructor Test
	{
		FBoxMinAndMax Box0;
		FBoxMinAndMax Box1(FIntVector(0), FIntVector(1));
		FBoxMinAndMax Box2(FIntVector(3, 2, 1), FIntVector(-1, -2, -3));
		FBoxMinAndMax Box3(Box2);
		FBoxMinAndMax Box4(FIntVector(-3, 2, -1), FIntVector(1, -2, 3));

		TestEqual(TEXT("Default Constructor Min == 0"), Box0.Min, FIntVector(0));
		TestEqual(TEXT("Default Constructor Max == 0"), Box0.Max, FIntVector(0));
		TestEqual(TEXT("Constructor (0,1) Min == 0"), Box1.Min, FIntVector(0));
		TestEqual(TEXT("Constructor (0,1) Max == 1"), Box1.Max, FIntVector(1));
		TestEqual(TEXT("Constructor ((3,2,1), (-1,-2,-3)) Min == (-1,-2,-3)"), Box2.Min, FIntVector(-1, -2, -3));
		TestEqual(TEXT("Constructor ((3,2,1), (-1,-2,-3)) Max == (3,2,1)"), Box2.Max, FIntVector(3, 2, 1));
		TestEqual(TEXT("Copy Constructor of ((3,2,1), (-1,-2,-3)) Min == (-1,-2,-3)"), Box3.Min, FIntVector(-1, -2, -3));
		TestEqual(TEXT("Copy Constructor of ((3,2,1), (-1,-2,-3)) Max == (3,2,1)"), Box3.Max, FIntVector(3, 2, 1));
		TestEqual(TEXT("Constructor ((-3,2,-1), (1,-2,3)) Min == (-3,-2,-1)"), Box4.Min, FIntVector(-3, -2, -1));
		TestEqual(TEXT("Constructor ((-3,2,-1), (1,-2,3)) Max == (1,2,3)"), Box4.Max, FIntVector(1, 2, 3));
	}

	// Size Test
	{
		FBoxMinAndMax Box0(FIntVector(0), FIntVector(0));
		FBoxMinAndMax Box1(FIntVector(0), FIntVector(1));
		FBoxMinAndMax Box2(FIntVector(0), FIntVector(-1));
		FBoxMinAndMax Box3(FIntVector(-1), FIntVector(0));
		FBoxMinAndMax Box4(FIntVector(1), FIntVector(0));
		FBoxMinAndMax Box5(FIntVector(0, 0, 0), FIntVector(3, 4, 5));
		FBoxMinAndMax Box6(FIntVector(3, 4, 5), FIntVector(-5, -4, -3));

		TestEqual(TEXT("Box(0, 0).GetSize() == (0,0,0)"), Box0.GetSize(), FIntVector(0));
		TestEqual(TEXT("Box(0, 1).GetSize() == (1,1,1)"), Box1.GetSize(), FIntVector(1));
		TestEqual(TEXT("Box(0, -1).GetSize() == (1,1,1)"), Box2.GetSize(), FIntVector(1));
		TestEqual(TEXT("Box(-1, 0).GetSize() == (1,1,1)"), Box3.GetSize(), FIntVector(1));
		TestEqual(TEXT("Box(1, 0).GetSize() == (1,1,1)"), Box4.GetSize(), FIntVector(1));
		TestEqual(TEXT("Box(0, (3, 4, 5)).GetSize() == (3,4,5)"), Box5.GetSize(), FIntVector(3, 4, 5));
		TestEqual(TEXT("Box((3, 4, 5), (-5, -4, -3)).GetSize() == (8,8,8)"), Box6.GetSize(), FIntVector(8));
	}

	// Overlap Test
	{
		FBoxMinAndMax Box1(FIntVector(-2, -3, -4), FIntVector(2, 3, 4));
		FBoxMinAndMax Box2(FIntVector(3, 0, 0), FIntVector(5, 5, 5));
		FBoxMinAndMax Box3(FIntVector(2, 3, 0), FIntVector(5, 5, 5));
		FBoxMinAndMax Box4(FIntVector(2, 2, 0), FIntVector(5, 5, 5));
		FBoxMinAndMax Box5(FIntVector(1, 2, 0), FIntVector(5, 5, 5));
		FBoxMinAndMax Box6(FIntVector(-3, 0, 0), FIntVector(-5, -5, -5));
		FBoxMinAndMax Box7(FIntVector(-2, -2, 0), FIntVector(-5, -5, -5));
		FBoxMinAndMax Box8(FIntVector(-1, -2, 0), FIntVector(-5, -5, -5));

		TestTrue(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((-2,-3,-4), (2,3,4)) overlap"), FBoxMinAndMax::Overlap(Box1, Box1));
		TestFalse(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((3,0,0), (5,5,5)) don't overlap"), FBoxMinAndMax::Overlap(Box1, Box2));
		TestFalse(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((2,3,0), (5,5,5)) don't overlap"), FBoxMinAndMax::Overlap(Box1, Box3));
		TestFalse(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((2,2,0), (5,5,5)) don't overlap"), FBoxMinAndMax::Overlap(Box1, Box4));
		TestTrue(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((1,2,0), (5,5,5)) overlap"), FBoxMinAndMax::Overlap(Box1, Box5));
		TestFalse(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((-3,0,0), (-5,-5,-5)) don't overlap"), FBoxMinAndMax::Overlap(Box1, Box6));
		TestFalse(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((-2,-2,0), (-5,-5,-5)) don't overlap"), FBoxMinAndMax::Overlap(Box1, Box7));
		TestTrue(TEXT("Box((-2,-3,-4), (2,3,4)) and Box((-1,-2,0), (-5,-5,-5)) overlap"), FBoxMinAndMax::Overlap(Box1, Box8));

		FBoxMinAndMax BoxA(FIntVector(0), FIntVector(1));
		FBoxMinAndMax BoxB(FIntVector(-2), FIntVector(5));
		TestTrue(TEXT("Box(0, 1) and Box(-2, 5) overlap"), FBoxMinAndMax::Overlap(BoxA, BoxB));
	}

	// Rotation Test
	{
		FBoxMinAndMax Box0(FIntVector(0), FIntVector(1));
		FBoxMinAndMax RotBox0N = Rotate(Box0, EDoorDirection::North);
		FBoxMinAndMax RotBox0E = Rotate(Box0, EDoorDirection::East);
		FBoxMinAndMax RotBox0S = Rotate(Box0, EDoorDirection::South);
		FBoxMinAndMax RotBox0W = Rotate(Box0, EDoorDirection::West);

		TestEqual(TEXT("Rotate(Box(0,1), N).Min == 0"), RotBox0N.Min, FIntVector(0));
		TestEqual(TEXT("Rotate(Box(0,1), N).Max == 1"), RotBox0N.Max, FIntVector(1));
		TestEqual(TEXT("Rotate(Box(0,1), E).Min == 0"), RotBox0E.Min, FIntVector(0));
		TestEqual(TEXT("Rotate(Box(0,1), E).Max == 1"), RotBox0E.Max, FIntVector(1));
		TestEqual(TEXT("Rotate(Box(0,1), S).Min == 0"), RotBox0S.Min, FIntVector(0));
		TestEqual(TEXT("Rotate(Box(0,1), S).Max == 1"), RotBox0S.Max, FIntVector(1));
		TestEqual(TEXT("Rotate(Box(0,1), W).Min == 0"), RotBox0W.Min, FIntVector(0));
		TestEqual(TEXT("Rotate(Box(0,1), W).Max == 1"), RotBox0W.Max, FIntVector(1));

		FBoxMinAndMax Box1(FIntVector(-1, 0, -1), FIntVector(3, 1, 1));
		FBoxMinAndMax RotBox1N = Rotate(Box1, EDoorDirection::North);
		FBoxMinAndMax RotBox1E = Rotate(Box1, EDoorDirection::East);
		FBoxMinAndMax RotBox1S = Rotate(Box1, EDoorDirection::South);
		FBoxMinAndMax RotBox1W = Rotate(Box1, EDoorDirection::West);

		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), N).Min == (-1,0,-1)"), RotBox1N.Min, FIntVector(-1, 0, -1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), N).Max == (3,1,1)"), RotBox1N.Max, FIntVector(3, 1, 1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), E).Min == (0,-1,-1)"), RotBox1E.Min, FIntVector(0, -1, -1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), E).Max == (1,3,1)"), RotBox1E.Max, FIntVector(1, 3, 1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), S).Min == (-2,0,-1)"), RotBox1S.Min, FIntVector(-2, 0, -1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), S).Max == (2,1,1)"), RotBox1S.Max, FIntVector(2, 1, 1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), W).Min == (0,-2,-1)"), RotBox1W.Min, FIntVector(0, -2, -1));
		TestEqual(TEXT("Rotate(Box((-1,0,-1), (3,1,1)), W).Max == (1,2,1)"), RotBox1W.Max, FIntVector(1, 2, 1));
	}

	// IsInside(FBoxMinAndMax) Test
	{
		FBoxMinAndMax Bounds(FIntVector(-4, -5, -6), FIntVector(7, 8, 9));
		FBoxMinAndMax Box(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Completely inside, no coincident face
		TestTrue(*FString::Printf(TEXT("Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Positive X
		Box += FIntVector(6, 0, 0); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[X+,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(1, 0, 0); // Intersecting
		TestFalse(*FString::Printf(TEXT("[X+,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(1, 0, 0); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[X+,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Reset
		Box = FBoxMinAndMax(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Positive Y
		Box += FIntVector(0, 6, 0); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Y+,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 1, 0); // Intersecting
		TestFalse(*FString::Printf(TEXT("[Y+,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 1, 0); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Y+,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Reset
		Box = FBoxMinAndMax(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Positive Z
		Box += FIntVector(0, 0, 6); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Z+,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 0, 1); // Intersecting
		TestFalse(*FString::Printf(TEXT("[Z+,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 0, 1); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Z+,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Reset
		Box = FBoxMinAndMax(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Negative X
		Box += FIntVector(-3, 0, 0); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[X-,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(-1, 0, 0); // Intersecting
		TestFalse(*FString::Printf(TEXT("[X-,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(-1, 0, 0); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[X-,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Reset
		Box = FBoxMinAndMax(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Negative Y
		Box += FIntVector(0, -4, 0); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Y-,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, -2, 0); // Intersecting
		TestFalse(*FString::Printf(TEXT("[Y-,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, -1, 0); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Y-,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));

		// Reset
		Box = FBoxMinAndMax(FIntVector(-1, -1, -1), FIntVector(1, 2, 3));

		// Negative Z
		Box += FIntVector(0, 0, -5); // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Z-,A] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 0, -3); // Intersecting
		TestFalse(*FString::Printf(TEXT("[Z-,B] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
		Box += FIntVector(0, 0, -1); // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Z-,C] Box%s IsInside Bounds%s"), *Box.ToString(), *Bounds.ToString()), Bounds.IsInside(Box));
	}

	// IsInside(FIntVector) Test
	{
		FBoxMinAndMax Bounds(FIntVector(-4, -5, -6), FIntVector(7, 8, 9));
		FIntVector Cell {0};

		// Completely inside, no coincident face
		TestTrue(*FString::Printf(TEXT("Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Positive X
		Cell = {6, 0, 0}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[X+,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {7, 0, 0}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[X+,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Negative X
		Cell = {-4, 0, 0}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[X-,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {-5, 0, 0}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[X-,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Positive Y
		Cell = {0, 7, 0}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Y+,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {0, 8, 0}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Y+,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Negative Y
		Cell = {0, -5, 0}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Y-,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {0, -6, 0}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Y-,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Positive Z
		Cell = {0, 0, 8}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Z+,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {0, 0, 9}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Z+,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));

		// Negative Z
		Cell = {0, 0, -6}; // Inside but with coincident face
		TestTrue(*FString::Printf(TEXT("[Z-,A] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
		Cell = {0, 0, -7}; // Outside but with a coincident face
		TestFalse(*FString::Printf(TEXT("[Z-,B] Cell(%s) IsInside Bounds%s"), *Cell.ToString(), *Bounds.ToString()), Bounds.IsInside(Cell));
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
