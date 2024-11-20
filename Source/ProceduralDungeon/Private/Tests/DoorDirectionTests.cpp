/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDoorDirectionTest, "ProceduralDungeon.Types.DoorDirection", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FDoorDirectionTest::RunTest(const FString& Parameters)
{
	// Adding directions is correct
	{
		TestEqual(TEXT("North + North = North"), EDoorDirection::North + EDoorDirection::North, EDoorDirection::North);
		TestEqual(TEXT("North + East = East"), EDoorDirection::North + EDoorDirection::East, EDoorDirection::East);
		TestEqual(TEXT("North + South = South"), EDoorDirection::North + EDoorDirection::South, EDoorDirection::South);
		TestEqual(TEXT("North + West = West"), EDoorDirection::North + EDoorDirection::West, EDoorDirection::West);

		TestEqual(TEXT("East + North = East"), EDoorDirection::East + EDoorDirection::North, EDoorDirection::East);
		TestEqual(TEXT("East + East = South"), EDoorDirection::East + EDoorDirection::East, EDoorDirection::South);
		TestEqual(TEXT("East + South = West"), EDoorDirection::East + EDoorDirection::South, EDoorDirection::West);
		TestEqual(TEXT("East + West = North"), EDoorDirection::East + EDoorDirection::West, EDoorDirection::North);

		TestEqual(TEXT("South + North = South"), EDoorDirection::South + EDoorDirection::North, EDoorDirection::South);
		TestEqual(TEXT("South + East = West"), EDoorDirection::South + EDoorDirection::East, EDoorDirection::West);
		TestEqual(TEXT("South + South = North"), EDoorDirection::South + EDoorDirection::South, EDoorDirection::North);
		TestEqual(TEXT("South + West = East"), EDoorDirection::South + EDoorDirection::West, EDoorDirection::East);

		TestEqual(TEXT("West + North = West"), EDoorDirection::West + EDoorDirection::North, EDoorDirection::West);
		TestEqual(TEXT("West + East = North"), EDoorDirection::West + EDoorDirection::East, EDoorDirection::North);
		TestEqual(TEXT("West + South = East"), EDoorDirection::West + EDoorDirection::South, EDoorDirection::East);
		TestEqual(TEXT("West + West = South"), EDoorDirection::West + EDoorDirection::West, EDoorDirection::South);
	}

	// Subtracting directions is correct
	{
		TestEqual(TEXT("North - North = North"), EDoorDirection::North - EDoorDirection::North, EDoorDirection::North);
		TestEqual(TEXT("North - East = West"), EDoorDirection::North - EDoorDirection::East, EDoorDirection::West);
		TestEqual(TEXT("North - South = South"), EDoorDirection::North - EDoorDirection::South, EDoorDirection::South);
		TestEqual(TEXT("North - West = East"), EDoorDirection::North - EDoorDirection::West, EDoorDirection::East);

		TestEqual(TEXT("East - North = East"), EDoorDirection::East - EDoorDirection::North, EDoorDirection::East);
		TestEqual(TEXT("East - East = North"), EDoorDirection::East - EDoorDirection::East, EDoorDirection::North);
		TestEqual(TEXT("East - South = West"), EDoorDirection::East - EDoorDirection::South, EDoorDirection::West);
		TestEqual(TEXT("East - West = South"), EDoorDirection::East - EDoorDirection::West, EDoorDirection::South);

		TestEqual(TEXT("South - North = South"), EDoorDirection::South - EDoorDirection::North, EDoorDirection::South);
		TestEqual(TEXT("South - East = East"), EDoorDirection::South - EDoorDirection::East, EDoorDirection::East);
		TestEqual(TEXT("South - South = North"), EDoorDirection::South - EDoorDirection::South, EDoorDirection::North);
		TestEqual(TEXT("South - West = West"), EDoorDirection::South - EDoorDirection::West, EDoorDirection::West);

		TestEqual(TEXT("West - North = West"), EDoorDirection::West - EDoorDirection::North, EDoorDirection::West);
		TestEqual(TEXT("West - East = South"), EDoorDirection::West - EDoorDirection::East, EDoorDirection::South);
		TestEqual(TEXT("West - South = East"), EDoorDirection::West - EDoorDirection::South, EDoorDirection::East);
		TestEqual(TEXT("West - West = North"), EDoorDirection::West - EDoorDirection::West, EDoorDirection::North);
	}

	// Negating directions are correct
	{
		TestEqual(TEXT("-North = North"), -EDoorDirection::North, EDoorDirection::North);
		TestEqual(TEXT("-East = West"), -EDoorDirection::East, EDoorDirection::West);
		TestEqual(TEXT("-South = South"), -EDoorDirection::South, EDoorDirection::South);
		TestEqual(TEXT("-West = East"), -EDoorDirection::West, EDoorDirection::East);
	}

	// Opposite directions are correct
	{
		TestEqual(TEXT("~North = South"), ~EDoorDirection::North, EDoorDirection::South);
		TestEqual(TEXT("~East = West"), ~EDoorDirection::East, EDoorDirection::West);
		TestEqual(TEXT("~South = North"), ~EDoorDirection::South, EDoorDirection::North);
		TestEqual(TEXT("~West = East"), ~EDoorDirection::West, EDoorDirection::East);
	}

	// Incrementing/decrementing directions are correct
	{
		EDoorDirection direction {EDoorDirection::North};
		TestEqual(TEXT("++North = East"), ++direction, EDoorDirection::East);
		TestEqual(TEXT("++East = South"), ++direction, EDoorDirection::South);
		TestEqual(TEXT("++South = West"), ++direction, EDoorDirection::West);
		TestEqual(TEXT("++West = North"), ++direction, EDoorDirection::North);

		direction = EDoorDirection::North;
		TestEqual(TEXT("--North = West"), --direction, EDoorDirection::West);
		TestEqual(TEXT("--West = South"), --direction, EDoorDirection::South);
		TestEqual(TEXT("--South = East"), --direction, EDoorDirection::East);
		TestEqual(TEXT("--East = North"), --direction, EDoorDirection::North);
	}

	// Boolean testing directions
	{
		TestFalse(TEXT("!North = false"), !EDoorDirection::North);
		TestFalse(TEXT("!East = false"), !EDoorDirection::East);
		TestFalse(TEXT("!South = false"), !EDoorDirection::South);
		TestFalse(TEXT("!West = false"), !EDoorDirection::West);
		TestTrue(TEXT("!NbDirection = true"), !EDoorDirection::NbDirection);
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
