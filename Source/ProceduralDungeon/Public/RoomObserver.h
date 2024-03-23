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
#include "RoomObserver.generated.h"

class ARoomLevel;
class AActor;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class URoomObserver : public UInterface
{
	GENERATED_BODY()
};

// Classes implementing this interface will get notified when any actor enters/exits the room this class belongs to.
// The observer needs to be registered first by calling ARoomLevel::RegisterObserver(...)
class PROCEDURALDUNGEON_API IRoomObserver
{
	GENERATED_BODY()

public:
	// This function is called when an actor enters the dungeon room where the observer is.
	// @param RoomLevel The room level blueprint this actor has entered. Same as the room where the observer is located. You can use the Room accessor from here.
	// @param Actor The actor that entered the room.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Observer")
	void OnActorEnterRoom(ARoomLevel* RoomLevel, AActor* Actor);

	// This function is called when an actor exits the dungeon room where the observer is.
	// @param RoomLevel The room level blueprint the actor has exited. Same as the room where the observer is located. You can use the Room accessor from here.
	// @param Actor The actor that exited the room.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Observer")
	void OnActorExitRoom(ARoomLevel* RoomLevel, AActor* Actor);
};
