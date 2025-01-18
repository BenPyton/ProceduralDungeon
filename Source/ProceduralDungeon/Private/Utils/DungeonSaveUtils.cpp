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

#include "Utils/DungeonSaveUtils.h"
#include "Serialization/StructuredArchive.h"
#include "Serialization/StructuredArchiveSlotBase.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Serialization/Formatters/JsonArchiveInputFormatter.h"
#include "Serialization/Formatters/JsonArchiveOutputFormatter.h"

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
	Obj->SerializeScriptProperties(Record.EnterField(TEXT("Properties")));
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
	auto ClassPath = FSoftObjectPath(Class);
	Slot << ClassPath;

	if (Slot.GetArchiveState().IsLoading())
	{
		// Resolve potential redirectors before trying to resolve the object
		ClassPath.FixupCoreRedirects();

		Class = Cast<UClass>(ClassPath.ResolveObject());
		if (nullptr == Class && !ClassPath.IsNull())
		{
			Class = Cast<UClass>(ClassPath.TryLoad());
		}

		if (nullptr == Class)
		{
			DungeonLog_Error("Failed to load class from path: %s", *ClassPath.ToString());
		}
	}
}

bool IsLoading(const FStructuredrchiveSlotBase& Slot)
{
	return Slot.GetArchiveState().IsLoading();
}

bool IsSaving(const FStructuredrchiveSlotBase& Slot)
{
	return Slot.GetArchiveState().IsSaving();
}
