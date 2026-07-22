// Copyright Benoit Pelletier 2025 - 2026 All Rights Reserved.
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
		TestEqual(TEXT("Constructor (1,2,3) South DoorTypeA :: Transform == (1,2,3) South"), Door2.Transform, FRoomTransform {{1, 2, 3}, EDoorDirection::South});
		TestEqual(TEXT("Constructor (1,2,3) South DoorTypeA :: Type == DoorTypeA"), Door2.Type, DoorTypeA.Get());

		TestTrue(TEXT("Copy Constructor of valid door must be valid too"), Door3.IsValid());
		TestEqual(TEXT("Copy Constructor must carry over transform"), Door3.Transform, Door2.Transform);
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
		TestEqual(TEXT("Opposite of North is South"), Door1.Transform.Rotation, EDoorDirection::South);
		TestEqual(TEXT("Opposite cell of (1,2,3)[North] is (2,2,3)"), Door1.Transform.Translation, {2, 2, 3});
		TestEqual(TEXT("Opposite type is the same"), Door1.Type, DoorTypeA.Get());
		TestTrue(TEXT("Opposite door is compatible with original"), FDoorDef::AreCompatible(Door0, Door1));
	}

	// GetTransformToTarget Test
	{
		FRoomTransform TransformA({1, 2, 3}, EDoorDirection::North);
		FRoomTransform TransformB({1, 2, 3}, EDoorDirection::East);
		FRoomTransform TransformC({1, 2, 3}, EDoorDirection::South);
		FRoomTransform TransformD({1, 2, 3}, EDoorDirection::West);
		FRoomTransform TransformE({1, 0, 0}, EDoorDirection::South);

		FDoorDef DoorA({1, 2, 3}, EDoorDirection::East, DoorTypeA.Get());

		FRoomTransform ExpectedATransformA({-1, 3, 0}, EDoorDirection::West);
		FRoomTransform ExpectedATransformB({0, 0, 0}, EDoorDirection::North);
		FRoomTransform ExpectedATransformC({3, 1, 0}, EDoorDirection::East);
		FRoomTransform ExpectedATransformD({2, 4, 0}, EDoorDirection::South);
		FRoomTransform ExpectedATransformE({3, -1, -3}, EDoorDirection::East);

		FRoomTransform ActualATransformA = DoorA.GetTransformToTarget(TransformA);
		FRoomTransform ActualATransformB = DoorA.GetTransformToTarget(TransformB);
		FRoomTransform ActualATransformC = DoorA.GetTransformToTarget(TransformC);
		FRoomTransform ActualATransformD = DoorA.GetTransformToTarget(TransformD);

		TestEqual(TEXT("Transformed door A moved at transform A must be correct."), ActualATransformA, ExpectedATransformA);
		TestEqual(TEXT("Transformed door A moved at transform B must be correct."), ActualATransformB, ExpectedATransformB);
		TestEqual(TEXT("Transformed door A moved at transform C must be correct."), ActualATransformC, ExpectedATransformC);
		TestEqual(TEXT("Transformed door A moved at transform D must be correct."), ActualATransformD, ExpectedATransformD);

		FDoorDef DoorB({0, 0, 0}, EDoorDirection::South, DoorTypeA.Get());

		FRoomTransform ExpectedBTransformA({1, 2, 3}, EDoorDirection::South);
		FRoomTransform ExpectedBTransformB({1, 2, 3}, EDoorDirection::West);
		FRoomTransform ExpectedBTransformC({1, 2, 3}, EDoorDirection::North);
		FRoomTransform ExpectedBTransformD({1, 2, 3}, EDoorDirection::East);
		FRoomTransform ExpectedBTransformE({1, 0, 0}, EDoorDirection::North);

		FRoomTransform ActualBTransformA = DoorB.GetTransformToTarget(TransformA);
		FRoomTransform ActualBTransformB = DoorB.GetTransformToTarget(TransformB);
		FRoomTransform ActualBTransformC = DoorB.GetTransformToTarget(TransformC);
		FRoomTransform ActualBTransformD = DoorB.GetTransformToTarget(TransformD);
		FRoomTransform ActualBTransformE = DoorB.GetTransformToTarget(TransformE);

		TestEqual(TEXT("Transformed door B moved at transform A must be correct."), ActualBTransformA, ExpectedBTransformA);
		TestEqual(TEXT("Transformed door B moved at transform B must be correct."), ActualBTransformB, ExpectedBTransformB);
		TestEqual(TEXT("Transformed door B moved at transform C must be correct."), ActualBTransformC, ExpectedBTransformC);
		TestEqual(TEXT("Transformed door B moved at transform D must be correct."), ActualBTransformD, ExpectedBTransformD);
		TestEqual(TEXT("Transformed door B moved at transform E must be correct."), ActualBTransformE, ExpectedBTransformE);
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
