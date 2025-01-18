/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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
#include "Interfaces/DungeonSaveInterface.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Utils/DungeonSaveUtils.h"
#include "Classes/DungeonSaveClasses.h"
#include "TestUtils.h"

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
