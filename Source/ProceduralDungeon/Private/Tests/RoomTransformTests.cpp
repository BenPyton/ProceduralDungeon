// Copyright Benoit Pelletier 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "ProceduralDungeonTypes.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRoomTransformTest, "ProceduralDungeon.Types.RoomTransform", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FRoomTransformTest::RunTest(const FString& Parameters)
{
	// Constructor Test
	{
		FRoomTransform TransformA;
		FRoomTransform TransformB(FRoomTransform::Invalid);
		FRoomTransform TransformC = {{1, 2, 3}, EDoorDirection::East};
		FRoomTransform TransformD(TransformC);

		TestTrue(TEXT("Default Constructor makes valid transform"), TransformA.IsValid());
		TestEqual(TEXT("Default Constructor makes identity transform"), TransformA, FRoomTransform::Identity);
		TestFalse(TEXT("Copy constructor of invalid transform must makes an invalid transform"), TransformB.IsValid());

		TestTrue(TEXT("Constructor (1,2,3) South is valid"), TransformC.IsValid());
		TestEqual(TEXT("Constructor (1,2,3) South Translation == (1,2,3)"), TransformC.Translation, FIntVector {1, 2, 3});
		TestEqual(TEXT("Constructor (1,2,3) South Rotation == East"), TransformC.Rotation, EDoorDirection::East);

		TestTrue(TEXT("Copy Constructor of valid door must be valid too"), TransformD.IsValid());
		TestEqual(TEXT("Copy Constructor must makes equal transforms"), TransformD, TransformC);
	}

	// Transform Vector Test
	{
		FIntVector BaseVector {-1, 3, 2};

		FRoomTransform TransformationA({1, 2, -1}, EDoorDirection::East);
		FRoomTransform TransformationB({-2, 1, 0}, EDoorDirection::South);
		FRoomTransform TransformationC({0, -1, 2}, EDoorDirection::West);

		FIntVector ExpectedVectorA {-2, 1, 1};
		FIntVector ExpectedVectorB {-1, -2, 2};
		FIntVector ExpectedVectorC {3, 0, 4};

		FIntVector ActualVectorA = TransformationA.Transform(BaseVector);
		FIntVector ActualVectorB = TransformationB.Transform(BaseVector);
		FIntVector ActualVectorC = TransformationC.Transform(BaseVector);
		TestEqual(TEXT("Vector Transformation A is correct"), ActualVectorA, ExpectedVectorA);
		TestEqual(TEXT("Vector Transformation B is correct"), ActualVectorB, ExpectedVectorB);
		TestEqual(TEXT("Vector Transformation C is correct"), ActualVectorC, ExpectedVectorC);

		FIntVector ActualInverseVectorA = TransformationA.InverseTransform(ExpectedVectorA);
		FIntVector ActualInverseVectorB = TransformationB.InverseTransform(ExpectedVectorB);
		FIntVector ActualInverseVectorC = TransformationC.InverseTransform(ExpectedVectorC);
		TestEqual(TEXT("Vector Inverse Transformation A is correct"), ActualInverseVectorA, BaseVector);
		TestEqual(TEXT("Vector Inverse Transformation B is correct"), ActualInverseVectorB, BaseVector);
		TestEqual(TEXT("Vector Inverse Transformation C is correct"), ActualInverseVectorC, BaseVector);
	}

	// Transform DoorDirection Test
	{
		FRoomTransform TransformationA({1, 2, -1}, EDoorDirection::East);
		FRoomTransform TransformationB({-2, 1, 0}, EDoorDirection::South);
		FRoomTransform TransformationC({0, -1, 2}, EDoorDirection::West);

		TestEqual(TEXT("North Transformation A is correct"), TransformationA.Transform(EDoorDirection::North), EDoorDirection::East);
		TestEqual(TEXT("North Transformation B is correct"), TransformationB.Transform(EDoorDirection::North), EDoorDirection::South);
		TestEqual(TEXT("North Transformation C is correct"), TransformationC.Transform(EDoorDirection::North), EDoorDirection::West);

		TestEqual(TEXT("East Transformation A is correct"), TransformationA.Transform(EDoorDirection::East), EDoorDirection::South);
		TestEqual(TEXT("East Transformation B is correct"), TransformationB.Transform(EDoorDirection::East), EDoorDirection::West);
		TestEqual(TEXT("East Transformation C is correct"), TransformationC.Transform(EDoorDirection::East), EDoorDirection::North);

		TestEqual(TEXT("South Transformation A is correct"), TransformationA.Transform(EDoorDirection::South), EDoorDirection::West);
		TestEqual(TEXT("South Transformation B is correct"), TransformationB.Transform(EDoorDirection::South), EDoorDirection::North);
		TestEqual(TEXT("South Transformation C is correct"), TransformationC.Transform(EDoorDirection::South), EDoorDirection::East);

		TestEqual(TEXT("West Transformation A is correct"), TransformationA.Transform(EDoorDirection::West), EDoorDirection::North);
		TestEqual(TEXT("West Transformation B is correct"), TransformationB.Transform(EDoorDirection::West), EDoorDirection::East);
		TestEqual(TEXT("West Transformation C is correct"), TransformationC.Transform(EDoorDirection::West), EDoorDirection::South);

		TestEqual(TEXT("North Inverse Transformation A is correct"), TransformationA.InverseTransform(EDoorDirection::North), EDoorDirection::West);
		TestEqual(TEXT("North Inverse Transformation B is correct"), TransformationB.InverseTransform(EDoorDirection::North), EDoorDirection::South);
		TestEqual(TEXT("North Inverse Transformation C is correct"), TransformationC.InverseTransform(EDoorDirection::North), EDoorDirection::East);

		TestEqual(TEXT("East Inverse Transformation A is correct"), TransformationA.InverseTransform(EDoorDirection::East), EDoorDirection::North);
		TestEqual(TEXT("East Inverse Transformation B is correct"), TransformationB.InverseTransform(EDoorDirection::East), EDoorDirection::West);
		TestEqual(TEXT("East Inverse Transformation C is correct"), TransformationC.InverseTransform(EDoorDirection::East), EDoorDirection::South);

		TestEqual(TEXT("South Inverse Transformation A is correct"), TransformationA.InverseTransform(EDoorDirection::South), EDoorDirection::East);
		TestEqual(TEXT("South Inverse Transformation B is correct"), TransformationB.InverseTransform(EDoorDirection::South), EDoorDirection::North);
		TestEqual(TEXT("South Inverse Transformation C is correct"), TransformationC.InverseTransform(EDoorDirection::South), EDoorDirection::West);

		TestEqual(TEXT("West Inverse Transformation A is correct"), TransformationA.InverseTransform(EDoorDirection::West), EDoorDirection::South);
		TestEqual(TEXT("West Inverse Transformation B is correct"), TransformationB.InverseTransform(EDoorDirection::West), EDoorDirection::East);
		TestEqual(TEXT("West Inverse Transformation C is correct"), TransformationC.InverseTransform(EDoorDirection::West), EDoorDirection::North);
	}

	// Transform Transform Test
	{
		FRoomTransform BaseTransform({1, 2, 3}, EDoorDirection::North);

		FRoomTransform TransformationA({1, 2, 3}, EDoorDirection::East);
		FRoomTransform TransformationB({1, 2, 3}, EDoorDirection::South);
		FRoomTransform TransformationC({1, 2, 3}, EDoorDirection::West);

		FRoomTransform ExpectedTransformA({-1, 3, 6}, EDoorDirection::East);
		FRoomTransform ExpectedTransformB({0, 0, 6}, EDoorDirection::South);
		FRoomTransform ExpectedTransformC({3, 1, 6}, EDoorDirection::West);

		FRoomTransform ActualTransformA = TransformationA.Transform(BaseTransform);
		FRoomTransform ActualTransformB = TransformationB.Transform(BaseTransform);
		FRoomTransform ActualTransformC = TransformationC.Transform(BaseTransform);
		TestEqual(TEXT("Transform Transformation A is correct"), ActualTransformA, ExpectedTransformA);
		TestEqual(TEXT("Transform Transformation B is correct"), ActualTransformB, ExpectedTransformB);
		TestEqual(TEXT("Transform Transformation C is correct"), ActualTransformC, ExpectedTransformC);

		FRoomTransform ActualInverseTransformA = TransformationA.InverseTransform(ExpectedTransformA);
		FRoomTransform ActualInverseTransformB = TransformationB.InverseTransform(ExpectedTransformB);
		FRoomTransform ActualInverseTransformC = TransformationC.InverseTransform(ExpectedTransformC);
		TestEqual(TEXT("Transform Inverse Transformation A is correct"), ActualInverseTransformA, BaseTransform);
		TestEqual(TEXT("Transform Inverse Transformation B is correct"), ActualInverseTransformB, BaseTransform);
		TestEqual(TEXT("Transform Inverse Transformation C is correct"), ActualInverseTransformC, BaseTransform);
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
