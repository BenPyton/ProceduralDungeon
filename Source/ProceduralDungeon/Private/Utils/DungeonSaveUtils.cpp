// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "Utils/DungeonSaveUtils.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/StructuredArchive.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Serialization/Formatters/JsonArchiveInputFormatter.h"
#include "Serialization/Formatters/JsonArchiveOutputFormatter.h"
#include "UObject/Class.h"

#if UE_VERSION_NEWER_THAN(5, 0, 0)
	#include "Serialization/StructuredArchiveSlotBase.h"
#endif

TUniquePtr<FArchiveFormatterType> CreateArchiveFormatterFromArchive(FArchive& Ar, bool bTextFormat)
{
#if !UE_BUILD_SHIPPING && WITH_TEXT_ARCHIVE_SUPPORT
	if (bTextFormat)
	{
		if (Ar.IsSaving())
			return MakeUnique<FJsonArchiveOutputFormatter>(Ar);
		else
			return MakeUnique<FJsonArchiveInputFormatter>(Ar);
	}
	else
#endif
	{
		return MakeUnique<FBinaryArchiveFormatter>(Ar);
	}
}

bool SerializeUObject(FStructuredArchive::FRecord& Record, UObject* Obj, bool bIsLoading)
{
	check(IsValid(Obj));

	const bool bImplementsDungeonSave = Obj->Implements<UDungeonSaveInterface>();

	// Allow modification of saved variables just before serializing into the saved dungeon.
	if (bImplementsDungeonSave)
		IDungeonSaveInterface::Execute_DungeonPreSerialize(Obj, bIsLoading);

	bool bSuccess = true;
	Obj->SerializeScriptProperties(Record.EnterField(AR_FIELD_NAME("Properties")));
	if (auto* SaveableObj = Cast<IDungeonCustomSerialization>(Obj))
	{
		bSuccess &= SaveableObj->SerializeObject(Record, bIsLoading);
	}

	// Allow some setup code right after the deserialization of its properties from the saved dungeon.
	if (bImplementsDungeonSave)
		IDungeonSaveInterface::Execute_DungeonPostSerialize(Obj, bIsLoading);

	return bSuccess;
}

bool SerializeUObject(FArchive& Ar, UObject* Obj, bool bIsLoading, bool bTextFormat)
{
	TUniquePtr<FArchiveFormatterType> Formatter = CreateArchiveFormatterFromArchive(Ar, bTextFormat);
	FStructuredArchive StructuredArchive(*Formatter);
	//FStructuredArchiveFromArchive StructuredArchive(Ar);
	FStructuredArchive::FRecord Record = StructuredArchive.Open().EnterRecord();
	SerializeUObject(Record, Obj, bIsLoading);
	return true;
}

bool SerializeUObject(TArray<uint8>& Data, UObject* Obj, bool bIsLoading, bool bTextFormat)
{
	TSharedPtr<FArchive> Ar {nullptr};
	if (bIsLoading)
	{
		Ar = MakeShared<FMemoryReader>(Data);
	}
	else
	{
		Ar = MakeShared<FMemoryWriter>(Data);
	}

	check(Ar != nullptr);

	FObjectAndNameAsStringProxyArchive Archive(*Ar, true);
	Archive.ArIsSaveGame = true;

	SerializeUObject(Archive, Obj, bIsLoading, bTextFormat);
	return true;
}

void SerializeUClass(FStructuredArchiveSlot Slot, UClass*& Class)
{
	SerializeUObjectRef(Slot, Class);
}

void SerializeUObjectRef(FStructuredArchiveSlot Slot, UObject*& Object)
{
	auto ObjPath = FSoftObjectPath(Object);
	Slot << ObjPath;

	if (Slot.GetArchiveState().IsLoading())
	{
		// Resolve potential redirectors before trying to resolve the object
		ObjPath.FixupCoreRedirects();

		Object = ObjPath.ResolveObject();
		if (nullptr == Object && !ObjPath.IsNull())
		{
			Object = ObjPath.TryLoad();
		}

		if (nullptr == Object)
		{
			DungeonLog_Error("Failed to load class from path: %s", *ObjPath.ToString());
		}
	}
}

bool IsLoading(const FStructuredArchiveSlotBase& Slot)
{
	return Slot.GetArchiveState().IsLoading();
}

bool IsSaving(const FStructuredArchiveSlotBase& Slot)
{
	return Slot.GetArchiveState().IsSaving();
}
