// Copyright Benoit Pelletier 2024 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "ProceduralDungeonUtils.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonUtilsTest_WeightedMap, "ProceduralDungeon.Utils.WeightedMaps", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FDungeonUtilsTest_WeightedMap::RunTest(const FString& Parameters)
{
	// Built-in types test
	{
		TMap<int, int> WeightedMap = {
			{1, 0}, // Weight with 0 should never be returned
			{2, 1}, // The first non-zero weight should be return for index 0
			{3, 2}, // Weights greater than 1 should be returned for as much indices
			{4, 1}	// The last one should be return when index == total weights minus one
		}; // Out of bounds index should return default value

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
			{&a, 2},
			{&b, 0}, // Weight with 0 in middle should be skipped
			{&c, 1},
			{&d, 0} // The last one should not be returned if weight is 0
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonUtilsTest_Guid2Seed, "ProceduralDungeon.Utils.Guid2Seed.Simple", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)

bool FDungeonUtilsTest_Guid2Seed::RunTest(const FString& Parameters)
{
	// Guid to Random Seed Tests
	{
		FGuid Guid1(0x12345678U, 0x9ABCDEF0U, 0x0FEDCBA9U, 0x87654321U);
		FGuid Guid2(0x08F7E6D5U, 0xC4B3A291U, 0x192A3B4CU, 0x5D6E7F80U);
		int64 Salt1 = 1;
		int64 Salt2 = 2;

		TestTrue(TEXT("Same Guid and Salt should return same Seed (1)"), Random::Guid2Seed(Guid1, Salt1) == Random::Guid2Seed(Guid1, Salt1));
		TestTrue(TEXT("Same Guid and Salt should return same Seed (2)"), Random::Guid2Seed(Guid2, Salt2) == Random::Guid2Seed(Guid2, Salt2));
		TestTrue(TEXT("Same Guid but different Salt should return different Seeds (1)"), Random::Guid2Seed(Guid1, Salt1) != Random::Guid2Seed(Guid1, Salt2));
		TestTrue(TEXT("Same Guid but different Salt should return different Seeds (2)"), Random::Guid2Seed(Guid2, Salt1) != Random::Guid2Seed(Guid2, Salt2));
		TestTrue(TEXT("Different Guid but same Salt should return different Seeds (1)"), Random::Guid2Seed(Guid1, Salt1) != Random::Guid2Seed(Guid2, Salt1));
		TestTrue(TEXT("Different Guid but same Salt should return different Seeds (2)"), Random::Guid2Seed(Guid1, Salt2) != Random::Guid2Seed(Guid2, Salt2));
		TestTrue(TEXT("Different Guid and Salt should return different Seeds (1)"), Random::Guid2Seed(Guid1, Salt1) != Random::Guid2Seed(Guid2, Salt2));
		TestTrue(TEXT("Different Guid and Salt should return different Seeds (2)"), Random::Guid2Seed(Guid1, Salt2) != Random::Guid2Seed(Guid2, Salt1));
	}

	return true;
}

BEGIN_DEFINE_SPEC(FGuid2SeedStatisticalTests, "ProceduralDungeon.Utils.Guid2Seed.Stats", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)
struct FTestParams
{
	FGuid Guid;
	int32 NbElements;
	int32 NbSamples;
	double CriticalValue;
};

using SampleArrayType = int32[];

static TUniquePtr<SampleArrayType> GenerateSamples(const FTestParams& Params)
{
	TUniquePtr<SampleArrayType> Occurences(new int32[Params.NbElements] {0});
	for (int32 i = 0; i < Params.NbSamples; ++i)
	{
		// Create a random stream with only the salt modified.
		// This will simulate an actor in a room level generating its first random number
		// in each of the room instances (where only the room ID changes)
		FRandomStream RNG(Random::Guid2Seed(Params.Guid, i));
		Occurences[RNG.RandRange(0, Params.NbElements - 1)]++;
	}
	return Occurences;
}

static bool PassChiSquaredTest(const FTestParams& Params, SampleArrayType Samples)
{
	const double Expected = static_cast<double>(Params.NbSamples) / Params.NbElements;
	double X2 = 0;
	for (int i = 0; i < Params.NbElements; ++i)
	{
		const double Delta = Samples[i] - Expected;
		X2 += (Delta * Delta) / Expected;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Chi Squared result %d: %f / %f"), Params.NbElements, X2, Params.CriticalValue);
	return X2 < Params.CriticalValue;
}
END_DEFINE_SPEC(FGuid2SeedStatisticalTests)

void FGuid2SeedStatisticalTests::Define()
{
	Describe(TEXT("Chi Squared Test"), [this]()
	{
		FTestParams Params;
		Params.NbSamples = 1'000'000'000;

		TArray<FGuid> GuidsToTest = {
			FGuid(),
			FGuid(0x12345678U, 0x9ABCDEF0U, 0x0FEDCBA9U, 0x87654321U),
			FGuid(0x08F7E6D5U, 0xC4B3A291U, 0x192A3B4CU, 0x5D6E7F80U),
		};

		for (const auto& Guid : GuidsToTest)
		{
			Params.Guid = Guid;
			Describe(FString::Printf(TEXT("with Guid %s"), *Guid.ToString()), [this, Params]() mutable
			{
				// The test cases we want to check for each Guid.
				// First value is the number of elements in the generated samples.
				// Second value is the critical value for the Chi Squared test (with p-value of 5%)
				TArray<TTuple<int32, double>> TestCases;
				TestCases.Add(TTuple<int32, double>(10, 16.91898));
				TestCases.Add(TTuple<int32, double>(100, 123.22522));
				TestCases.Add(TTuple<int32, double>(1000, 1073.64265));

				for (const auto& TestCase : TestCases)
				{
					Params.NbElements = TestCase.Key;
					Params.CriticalValue = TestCase.Value;
					It(FString::Printf(TEXT("with %d elements"), Params.NbElements), [this, Params]()
					{
						TUniquePtr<SampleArrayType> Samples = GenerateSamples(Params);
						TestTrue(TEXT("Pass"), PassChiSquaredTest(Params, Samples.Get()));
					});
				}
			});
		}
	});
}

#endif //WITH_DEV_AUTOMATION_TESTS
