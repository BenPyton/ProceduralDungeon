// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "DoorType.h"
#include "UObject/Package.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDoorTypeTests, "ProceduralDungeon.Types.DoorType", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)

bool FDoorTypeTests::RunTest(const FString& Parameters)
{
	// Test DoorType Compatibility
	{

		CREATE_DATA_ASSET(UDoorType, TypeA);
		CREATE_DATA_ASSET(UDoorType, TypeB);
		CREATE_DATA_ASSET(UDoorType, TypeC);
		/* Default Compatibility Table:
		 *   A B C N
		 * A O X X X
		 * B X O X X
		 * C X X O X
		 * N X X X O
		 */
		TestTrue("TypeA should be compatible with itself", UDoorType::AreCompatible(TypeA.Get(), TypeA.Get()));
		TestFalse("TypeA should not be compatible with TypeB", UDoorType::AreCompatible(TypeA.Get(), TypeB.Get()));
		TestFalse("TypeA should not be compatible with TypeC", UDoorType::AreCompatible(TypeA.Get(), TypeC.Get()));
		TestFalse("TypeA should not be compatible with Null", UDoorType::AreCompatible(TypeA.Get(), nullptr));

		TestFalse("TypeB should not be compatible with TypeA", UDoorType::AreCompatible(TypeB.Get(), TypeA.Get()));
		TestTrue("TypeB should be compatible with itself", UDoorType::AreCompatible(TypeB.Get(), TypeB.Get()));
		TestFalse("TypeB should not be compatible with TypeC", UDoorType::AreCompatible(TypeB.Get(), TypeC.Get()));
		TestFalse("TypeB should not be compatible with Null", UDoorType::AreCompatible(TypeB.Get(), nullptr));

		TestFalse("TypeC should not be compatible with TypeA", UDoorType::AreCompatible(TypeC.Get(), TypeA.Get()));
		TestFalse("TypeC should not be compatible with TypeB", UDoorType::AreCompatible(TypeC.Get(), TypeB.Get()));
		TestTrue("TypeC should not be compatible with TypeC", UDoorType::AreCompatible(TypeC.Get(), TypeC.Get()));
		TestFalse("TypeC should not be compatible with Null", UDoorType::AreCompatible(TypeC.Get(), nullptr));

		TestFalse("Null should not be compatible with TypeA", UDoorType::AreCompatible(nullptr, TypeA.Get()));
		TestFalse("Null should not be compatible with TypeB", UDoorType::AreCompatible(nullptr, TypeB.Get()));
		TestFalse("Null should not be compatible with TypeC", UDoorType::AreCompatible(nullptr, TypeC.Get()));
		TestTrue("Null should be compatible with Null", UDoorType::AreCompatible(nullptr, nullptr));
		
		/* Test Compatibility Table:
		 *   A B C N
		 * A O O X X
		 * B O X X X
		 * C X X O X
		 * N X X X O
		 */
		TypeA->SetCompatibility({});
		TypeB->SetCompatibility({ TypeA.Get() });
		TypeB->SetCompatibleWithItself(false);
		TypeC->SetCompatibility({});

		TestTrue("TypeA should be compatible with itself", UDoorType::AreCompatible(TypeA.Get(), TypeA.Get()));
		TestTrue("TypeA should be compatible with TypeB", UDoorType::AreCompatible(TypeA.Get(), TypeB.Get()));
		TestFalse("TypeA should not be compatible with TypeC", UDoorType::AreCompatible(TypeA.Get(), TypeC.Get()));
		TestFalse("TypeA should not be compatible with Null", UDoorType::AreCompatible(TypeA.Get(), nullptr));

		TestTrue("TypeB should be compatible with TypeA", UDoorType::AreCompatible(TypeB.Get(), TypeA.Get()));
		TestFalse("TypeB should not be compatible with itself", UDoorType::AreCompatible(TypeB.Get(), TypeB.Get()));
		TestFalse("TypeB should not be compatible with TypeC", UDoorType::AreCompatible(TypeB.Get(), TypeC.Get()));
		TestFalse("TypeB should not be compatible with Null", UDoorType::AreCompatible(TypeB.Get(), nullptr));

		TestFalse("TypeC should not be compatible with TypeA", UDoorType::AreCompatible(TypeC.Get(), TypeA.Get()));
		TestFalse("TypeC should not be compatible with TypeB", UDoorType::AreCompatible(TypeC.Get(), TypeB.Get()));
		TestTrue("TypeC should be compatible with TypeC", UDoorType::AreCompatible(TypeC.Get(), TypeC.Get()));
		TestFalse("TypeC should not be compatible with Null", UDoorType::AreCompatible(TypeC.Get(), nullptr));

		TestFalse("Null should not be compatible with TypeA", UDoorType::AreCompatible(nullptr, TypeA.Get()));
		TestFalse("Null should not be compatible with TypeB", UDoorType::AreCompatible(nullptr, TypeB.Get()));
		TestFalse("Null should not be compatible with TypeC", UDoorType::AreCompatible(nullptr, TypeC.Get()));
		TestTrue("Null should be compatible with Null", UDoorType::AreCompatible(nullptr, nullptr));
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
