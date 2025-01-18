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

#pragma once

#include "CoreMinimal.h"
#include "ProceduralDungeonLog.h"

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

// Can't make it an operator<< since it already exists.
void SerializeUClass(FStructuredArchiveSlot Slot, UClass*& Class);

template <typename T>
void operator<<(FStructuredArchiveSlot Slot, TSubclassOf<T>& Class)
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

namespace
{
	using FStructuredrchiveSlotBase = UE::StructuredArchive::Private::FSlotBase;
}

bool IsLoading(const FStructuredrchiveSlotBase& Slot);
bool IsSaving(const FStructuredrchiveSlotBase& Slot);
