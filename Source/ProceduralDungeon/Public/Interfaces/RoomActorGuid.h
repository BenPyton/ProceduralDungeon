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
#include "RoomActorGuid.generated.h"

UINTERFACE(BlueprintType, Blueprintable, meta = (Tooltip = "Interface to access a custom Guid for actors saved within a dungeon."))
class URoomActorGuid : public UInterface
{
	GENERATED_BODY()
};

// Interface for all saveable actors placed in room levels
// The guid must be constant across game sessions to be able to save/load the actors.
// It can be placed on ActorComponents too, but the interface on the Actor itself will be prioritized.
// Only the first component found that implements the interface will be used. Make sure to have only one to prevent any confusions.
class PROCEDURALDUNGEON_API IRoomActorGuid
{
	GENERATED_BODY()

public:
	// Return the guid associated with this actor.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Room Actor Id")
	FGuid GetGuid() const;

	// Returns true if the actor should be included in the saved dungeon.
	// Returns false to just use a Guid without the need to include the actor in saved games.
	UFUNCTION(BlueprintNativeEvent, Category = "Room Actor Id")
	bool ShouldSaveActor() const;

	// Return the object implementing the IRoomActorGuid interface from the provided actor.
	// It can be implemented on the Actor itself or its components.
	// If both, the actor implementation will be returned.
	static UObject* GetImplementer(AActor* Actor);
};
