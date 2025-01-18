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

#include "CoreUObject.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "DungeonSaveClasses.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType, HideDropdown, meta = (HiddenNode))
class USaveTestObject : public UObject, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override
	{
		OrderOfExecution += (bIsLoading) ? TEXT("X") : TEXT("C");
		Record.EnterField(TEXT("NativeTest")) << TestSerializeObjectFunction;
		return true;
	}
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PreSaveDungeon_Implementation() override
	{
		OrderOfExecution += TEXT("A");
	}

	virtual void DungeonPreSerialize_Implementation(bool bIsLoading) override
	{
		OrderOfExecution += (bIsLoading) ? TEXT("W") : TEXT("B");
	}

	virtual void DungeonPostSerialize_Implementation(bool bIsLoading) override
	{
		OrderOfExecution += (bIsLoading) ? TEXT("Y") : TEXT("D");
	}

	virtual void PostLoadDungeon_Implementation() override
	{
		OrderOfExecution += TEXT("Z");
	}
	//~ End IDungeonSaveInterface Interface

public:
	UPROPERTY(SaveGame)
	int32 TestSaveGameFlag {0};

	int32 TestSerializeObjectFunction {0};

	UPROPERTY();
	FString OrderOfExecution {};
};
