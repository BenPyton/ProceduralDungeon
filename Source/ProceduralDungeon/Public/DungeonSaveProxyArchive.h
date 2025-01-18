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

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Templates/SubclassOf.h"
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

		UE_LOG(LogTemp, Warning, TEXT("Custom serialization of a SoftObjectPath!"));

		// If we have a defined core redirect, make sure that it's applied
		if (IsLoading() && !Value.IsNull())
		{
			Value.FixupCoreRedirects();
		}

		return *this;
	}
};
