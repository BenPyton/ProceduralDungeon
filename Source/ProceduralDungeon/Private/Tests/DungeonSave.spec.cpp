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
#include "ProceduralDungeonUtils.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Utils/DungeonSaveUtils.h"
#include "Classes/DungeonSaveClasses.h"
#include "TestUtils.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

DEFINE_SPEC(FDungeonSaveSpecs, "ProceduralDungeon.SaveLoad", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)

void FDungeonSaveSpecs::Define()
{
	Describe(TEXT("Interfaces"), [this]()
	{
		It(TEXT("DungeonSaveInterface"), [this]()
		{
			TStrongObjectPtr<USaveTestObject> TestSave(NewObject<USaveTestObject>(GetTransientPackage()));
			TStrongObjectPtr<USaveTestObject> TestLoad(NewObject<USaveTestObject>(GetTransientPackage()));

			TestSave->TestSaveGameFlag = 5;
			TestSave->TestSerializeObjectFunction = 8;

			TArray<uint8> SavedData {};
			SerializeUObject(SavedData, TestSave.Get(), false);

			TestEqual(TEXT("should have correct order of execution"), TestSave->OrderOfExecution, FString(TEXT("BCD")));
			TestTrue(TEXT("should have written data"), SavedData.Num() > 0);

			SerializeUObject(SavedData, TestLoad.Get(), true);

			TestEqual(TEXT("should have correct order of execution"), TestLoad->OrderOfExecution, FString(TEXT("WXY")));
			TestEqual(TEXT("should have retrieved data from SaveGame flag"), TestLoad->TestSaveGameFlag, TestSave->TestSaveGameFlag);
			TestEqual(TEXT("should have retrieved data from SerializedObject function"), TestLoad->TestSerializeObjectFunction, TestSave->TestSerializeObjectFunction);
		});
	});
}

#endif //WITH_DEV_AUTOMATION_TESTS
