// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "ProceduralDungeonTypes.h"
#include "DoorType.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDoorDefTest, "ProceduralDungeon.Types.DoorDef", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FDoorDefTest::RunTest(const FString& Parameters)
{
	CREATE_DATA_ASSET(UDoorType, DoorTypeA);
	CREATE_DATA_ASSET(UDoorType, DoorTypeB);

	// Constructor Test
	{
		FDoorDef Door0;
		FDoorDef Door1(FDoorDef::Invalid);
		FDoorDef Door2({1, 2, 3}, EDoorDirection::South, DoorTypeA.Get());
		FDoorDef Door3(Door2);

		TestTrue(TEXT("Default Constructor makes valid door"), Door0.IsValid());
		TestFalse(TEXT("Copy constructor of invalid door mus makes an invalid door"), Door1.IsValid());

		TestTrue(TEXT("Constructor (1,2,3) South DoorTypeA is valid"), Door2.IsValid());
		TestEqual(TEXT("Constructor (1,2,3) South DoorTypeA :: Position == (1,2,3)"), Door2.Position, {1, 2, 3});
		TestEqual(TEXT("Constructor (1,2,3) South DoorTypeA :: Direction == South"), Door2.Direction, EDoorDirection::South);
		TestEqual(TEXT("Constructor (1,2,3) South DoorTypeA :: Type == DoorTypeA"), Door2.Type, DoorTypeA.Get());

		TestTrue(TEXT("Copy Constructor of valid door must be valid too"), Door3.IsValid());
		TestEqual(TEXT("Copy Constructor must carry over position"), Door3.Position, Door2.Position);
		TestEqual(TEXT("Copy Constructor must carry over direction"), Door3.Direction, Door2.Direction);
		TestEqual(TEXT("Copy Constructor must carry over type"), Door3.Type, Door2.Type);
	}

	// Compatibility Test
	{
		FDoorDef Door0({0, 0, 0}, EDoorDirection::North, DoorTypeA.Get());
		FDoorDef Door1({1, 2, 3}, EDoorDirection::South, DoorTypeA.Get());
		FDoorDef Door2({1, 2, 3}, EDoorDirection::South, DoorTypeB.Get());
		FDoorDef Door3;

		TestTrue(TEXT("Door0 is compatible with Door1"), FDoorDef::AreCompatible(Door0, Door1));
		TestFalse(TEXT("Door0 is not compatible with Door2"), FDoorDef::AreCompatible(Door0, Door2));
		TestFalse(TEXT("Door0 is not compatible with Door3"), FDoorDef::AreCompatible(Door0, Door3));
		TestFalse(TEXT("Door1 is not compatible with Door2"), FDoorDef::AreCompatible(Door1, Door2));
		TestFalse(TEXT("Door1 is not compatible with Door3"), FDoorDef::AreCompatible(Door1, Door3));
		TestFalse(TEXT("Door2 is not compatible with Door3"), FDoorDef::AreCompatible(Door2, Door3));
	}

	// Opposite Test
	{
		FDoorDef Door0({1, 2, 3}, EDoorDirection::North, DoorTypeA.Get());
		FDoorDef Door1 = Door0.GetOpposite();

		TestTrue(TEXT("Opposite door is valid"), Door1.IsValid());
		TestEqual(TEXT("Opposite of North is South"), Door1.Direction, EDoorDirection::South);
		TestEqual(TEXT("Opposite cell of (1,2,3)[North] is (2,2,3)"), Door1.Position, {2, 2, 3});
		TestEqual(TEXT("Opposite type is the same"), Door1.Type, DoorTypeA.Get());
		TestTrue(TEXT("Opposite door is compatible with original"), FDoorDef::AreCompatible(Door0, Door1));
	}

	// Transform Test
	{
		FDoorDef Door0({1, 2, 3}, EDoorDirection::North, DoorTypeA.Get());
		FDoorDef TransformedDoor0({-1, 3, 6}, EDoorDirection::East, DoorTypeA.Get());
		FDoorDef TransformedDoor1({0, 0, 6}, EDoorDirection::South, DoorTypeA.Get());
		FDoorDef TransformedDoor2({3, 1, 6}, EDoorDirection::West, DoorTypeA.Get());

		TestEqual(TEXT("Transformation {(1, 2, 3), East} is correct"), FDoorDef::Transform(Door0, {1, 2, 3}, EDoorDirection::East), TransformedDoor0);
		TestEqual(TEXT("Transformation {(1, 2, 3), South} is correct"), FDoorDef::Transform(Door0, {1, 2, 3}, EDoorDirection::South), TransformedDoor1);
		TestEqual(TEXT("Transformation {(1, 2, 3), West} is correct"), FDoorDef::Transform(Door0, {1, 2, 3}, EDoorDirection::West), TransformedDoor2);

		TestEqual(TEXT("Inverse Transformation {(1, 2, 3), East} is correct"), FDoorDef::InverseTransform(TransformedDoor0, {1, 2, 3}, EDoorDirection::East), Door0);
		TestEqual(TEXT("Inverse Transformation {(1, 2, 3), South} is correct"), FDoorDef::InverseTransform(TransformedDoor1, {1, 2, 3}, EDoorDirection::South), Door0);
		TestEqual(TEXT("Inverse Transformation {(1, 2, 3), West} is correct"), FDoorDef::InverseTransform(TransformedDoor2, {1, 2, 3}, EDoorDirection::West), Door0);
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
