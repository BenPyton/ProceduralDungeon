/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "ReplicableObject.h"
#include "Templates/SubclassOf.h"
#include "RoomCustomData.generated.h"

class UActorComponent;

// Base class for user custom data embedded in room instances
UCLASS(Abstract, BlueprintType, Blueprintable)
class PROCEDURALDUNGEON_API URoomCustomData : public UReplicableObject
{
	GENERATED_BODY()

public:
	void CreateLevelComponent(class ARoomLevel* LevelActor) const;

private:
	// Component to create and attach on the Level Script Actor of the instanced room.
	UPROPERTY(EditAnywhere, Category = "Dungeon Rules", meta = (AllowAbstract = false, AllowPrivateAccess = true))
	TSubclassOf<UActorComponent> LevelComponent {nullptr};
};
