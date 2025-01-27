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

#include "ProceduralDungeonTypes.h"
#include "ReadOnlyRoom.generated.h"

class URoomData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint, Tooltip = "Allow access to only some members of Room instances during the generation process."))
class UReadOnlyRoom : public UInterface
{
	GENERATED_BODY()
};

// Interface to access some room instance's data during the generation process.
class PROCEDURALDUNGEON_API IReadOnlyRoom
{
	GENERATED_BODY()

public:
	// Returns the room data asset of this room instance.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual const URoomData* GetRoomData() const { return nullptr; }

	// Returns the unique ID (per-dungeon) of the room.
	// The first room has ID 0 and then it increases in the order of placed room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual int64 GetRoomID() const { return -1ll; }

	// Returns the world extents (half size) of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FIntVector GetPosition() const { return FIntVector::ZeroValue; }

	// Returns the world extents (half size) of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual EDoorDirection GetDirection() const { return EDoorDirection::North; }

	// Returns true if all the doors of this room are connected to other rooms.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual bool AreAllDoorsConnected() const { return false; }

	// Returns true if all the doors of this room are connected to other rooms.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual int CountConnectedDoors() const { return -1; }

	// Returns the world center position of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FVector GetBoundsCenter() const { return FVector::ZeroVector; }
	
	// Returns the world extents (half size) of the room.
	UFUNCTION(BlueprintCallable, Category = "Room")
	virtual FVector GetBoundsExtent() const { return FVector::ZeroVector; }
};
