// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPath.h"
#include "Serialization/StructuredArchive.h"

// Archive proxy specialized for the dungeon.
struct FDungeonSaveProxyArchive : public FObjectAndNameAsStringProxyArchive
{
public:
	FDungeonSaveProxyArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
		//ArNoDelta = true;
	}

	virtual FArchive& operator<<(FSoftObjectPath& Value) override
	{
		// Calls Value.SerializePath()
		FObjectAndNameAsStringProxyArchive::operator<<(Value);

		//UE_LOG(LogTemp, Warning, TEXT("Custom serialization of a SoftObjectPath!"));

		// If we have a defined core redirect, make sure that it's applied
		if (IsLoading() && !Value.IsNull())
		{
			Value.FixupCoreRedirects();
		}

		return *this;
	}
};
