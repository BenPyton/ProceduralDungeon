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
#include "RoomContainer.generated.h"

class URoom;
class URoomConnection;

UINTERFACE(MinimalAPI, NotBlueprintable, NotBlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class URoomContainer : public UInterface
{
	GENERATED_BODY()
};

/**
 * Common interface for all containers that holds rooms and their connections.
 * Currently used to get back references in URoom and URoomConnection when loaded from a saved dungeon.
 */
class PROCEDURALDUNGEON_API IRoomContainer
{
	GENERATED_BODY()

public:
	virtual URoom* GetRoomByIndex(int64 Index) const = 0;
	virtual URoomConnection* GetConnectionByIndex(int32 Index) const = 0;
};
