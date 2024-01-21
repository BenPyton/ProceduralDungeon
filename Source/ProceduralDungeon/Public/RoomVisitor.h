/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
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
#include "RoomVisitor.generated.h"

class ARoomLevel;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class URoomVisitor : public UInterface
{
	GENERATED_BODY()
};

// Allows notify when entering/exiting a dungeon room.
class PROCEDURALDUNGEON_API IRoomVisitor
{
	GENERATED_BODY()

public:
	// This function is called when the actor enters a dungeon room.
	// @param RoomLevel The room level blueprint this actor has entered. You can use the Room accessor from here.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Visitor")
	void OnRoomEnter(ARoomLevel* RoomLevel);

	// This function is called when the actor exits a dungeon room.
	// @param RoomLevel The room level blueprint this actor has exited. You can use the Room accessor from here.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Visitor")
	void OnRoomExit(ARoomLevel* RoomLevel);
};
