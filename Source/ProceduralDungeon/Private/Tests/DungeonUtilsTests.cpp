/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
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
#include "ProceduralDungeonUtils.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonUtilsTest, "ProceduralDungeon.Utils.WeightedMaps", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FDungeonUtilsTest::RunTest(const FString& Parameters)
{
	// Built-in types test
	{
		TMap<int, int> WeightedMap = {
			{ 1, 0 },	// Weight with 0 should never be returned
			{ 2, 1 },	// The first non-zero weight should be return for index 0
			{ 3, 2 },	// Weights greater than 1 should be returned for as much indices
			{ 4, 1 }	// The last one should be return when index == total weights minus one
		};				// Out of bounds index should return default value

		TestEqual(TEXT("Total Weights"), Dungeon::GetTotalWeight(WeightedMap), 4);
		TestEqual(TEXT("Weighted value at -1"), Dungeon::GetWeightedAt(WeightedMap, -1), 0); // negative index should return default value
		TestEqual(TEXT("Weighted value at 0"), Dungeon::GetWeightedAt(WeightedMap, 0), 2);
		TestEqual(TEXT("Weighted value at 1"), Dungeon::GetWeightedAt(WeightedMap, 1), 3);
		TestEqual(TEXT("Weighted value at 2"), Dungeon::GetWeightedAt(WeightedMap, 2), 3);
		TestEqual(TEXT("Weighted value at 3"), Dungeon::GetWeightedAt(WeightedMap, 3), 4);
		TestEqual(TEXT("Weighted value at 4"), Dungeon::GetWeightedAt(WeightedMap, 4), 0); // default int is 0
	}

	// Pointer test
	{
		int a = 1;
		int b = 2;
		int c = 3;
		int d = 4;

		TMap<int*, int> WeightedMap = {
			{ &a, 2 },
			{ &b, 0 },	// Weight with 0 in middle should be skipped
			{ &c, 1 },
			{ &d, 0 }	// The last one should not be returned if weight is 0
		};

		TestEqual(TEXT("Total Weights"), Dungeon::GetTotalWeight(WeightedMap), 3);
		TestEqual(TEXT("Weighted pointer at -1"), Dungeon::GetWeightedAt(WeightedMap, -1), (int*)nullptr);
		TestEqual(TEXT("Weighted pointer at 0"), Dungeon::GetWeightedAt(WeightedMap, 0), &a);
		TestEqual(TEXT("Weighted pointer at 1"), Dungeon::GetWeightedAt(WeightedMap, 1), &a);
		TestEqual(TEXT("Weighted pointer at 2"), Dungeon::GetWeightedAt(WeightedMap, 2), &c);
		TestEqual(TEXT("Weighted pointer at 3"), Dungeon::GetWeightedAt(WeightedMap, 3), (int*)nullptr);
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
