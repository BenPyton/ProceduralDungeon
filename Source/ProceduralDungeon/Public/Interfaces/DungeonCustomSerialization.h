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
#include "UObject/Interface.h"
#include "DungeonCustomSerialization.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable, NotBlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UDungeonCustomSerialization : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROCEDURALDUNGEON_API IDungeonCustomSerialization
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Serialize non-trivial object properties (e.g. UObject pointers)
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) = 0;

	// Fixup object references after loading
	virtual bool FixupReferences(UObject* Context) { return true; }

	// Calls FixupReferences on Obj and its subobjects.
	static bool DispatchFixupReferences(UObject* Obj, UObject* Context);
};
