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
#include "DungeonSaveInterface.generated.h"

UINTERFACE(BlueprintType, Blueprintable)
class UDungeonSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to add some events to the saved actors/objects during the save/load process of the dungeon.
 */
class PROCEDURALDUNGEON_API IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	// Called just before serializing this object into the dungeon save.
	// Useful to initialize some saved variables based on actor states.
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void DungeonPreSerialize(bool bIsLoading);

	// Called just after deserializing this object from the dungeon save
	// Useful to initialize some actor states based on saved variables.
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void DungeonPostSerialize(bool bIsLoading);

	// Called first before saving the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void PreSaveDungeon();

	// Called last after loading the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Procedural Dungeon")
	void PostLoadDungeon();

	static void DispatchPreSaveEvent(UObject* Obj);
	static void DispatchPostLoadEvent(UObject* Obj);
};
