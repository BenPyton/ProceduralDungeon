// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Utils/CompatUtils.h"
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
		Record.EnterField(AR_FIELD_NAME("NativeTest")) << TestSerializeObjectFunction;
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
