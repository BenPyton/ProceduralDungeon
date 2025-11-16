// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralDungeonLog.h"
#include "Utils/CompatUtils.h"

TUniquePtr<FArchiveFormatterType> CreateArchiveFormatterFromArchive(FArchive& Ar, bool bTextFormat = true);

bool SerializeUObject(FStructuredArchive::FRecord& Record, UObject* Obj, bool bIsLoading);
bool SerializeUObject(FArchive& Ar, UObject* Obj, bool bIsLoading, bool bTextFormat = true);
bool SerializeUObject(TArray<uint8>& Data, UObject* Obj, bool bIsLoading, bool bTextFormat = true);

template<typename T UE_REQUIRES(TIsDerivedFrom<T, UObject>::Value)>
bool SerializeUObjectArray(FStructuredArchive::FRecord& ParentRecord, FArchiveFieldName RecordName, TArray<T*>& Array, bool bIsLoading, UObject* Owner = nullptr)
{
	int32 ArraySize = -1;
	if (!bIsLoading)
	{
		ArraySize = Array.Num();
	}

	FStructuredArchiveArray ArrayRecord = ParentRecord.EnterArray(RecordName, ArraySize);

	if (bIsLoading)
	{
		checkf(Array.Num() == 0, TEXT("Array has not been emptied before loading it from save!"));
		Array.Empty(ArraySize);
	}

	bool bSuccess = true;
	for (int i = 0; i < ArraySize; ++i)
	{
		T* Element = nullptr;
		if (bIsLoading)
		{
			// Create new object instance when loading
			Element = NewObject<T>(Owner);
			Array.Add(Element);
		}
		else
		{
			Element = Array[i];
		}

		FStructuredArchiveRecord ElementRecord = ArrayRecord.EnterElement().EnterRecord();
		bSuccess &= SerializeUObject(ElementRecord, Element, bIsLoading);
	}

	return bSuccess;
}

void SerializeUObjectRef(FStructuredArchiveSlot Slot, UObject*& Object);

template<typename T UE_REQUIRES(TIsDerivedFrom<T, UObject>::Value)>
void SerializeUObjectRef(FStructuredArchiveSlot Slot, T*& Object)
{
	UObject* Obj = Object;
	SerializeUObjectRef(Slot, Obj);
	Object = Cast<T>(Obj);
}

// Can't make it an operator<< since it already exists.
void SerializeUClass(FStructuredArchiveSlot Slot, UClass*& Class);

#if UE_VERSION_OLDER_THAN(5, 4, 0)
template<typename T>
void operator<<(FStructuredArchiveSlot Slot, TSubclassOf<T>& Class)
{
	auto ClassPath = FSoftObjectPath(Class);
	Slot << ClassPath;

	if (Slot.GetArchiveState().IsLoading() && !ClassPath.IsNull())
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
#endif

namespace
{
#if UE_VERSION_OLDER_THAN(5, 0, 0)
	using FStructuredArchiveSlotBase = UE4StructuredArchive_Private::FSlotBase;
#else
	using FStructuredArchiveSlotBase = UE::StructuredArchive::Private::FSlotBase;
#endif
} //namespace

bool IsLoading(const FStructuredArchiveSlotBase& Slot);
bool IsSaving(const FStructuredArchiveSlotBase& Slot);
