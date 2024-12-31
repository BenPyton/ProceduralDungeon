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
#include "Components/ActorComponent.h"
#include "DeterministicRandomComponent.generated.h"

// A simple component that adds a RandomStream to any actor placed in a room level.
// It will uses the actor's guid (provided by a IRoomActorGuid interface) and the owning room's ID
// to generate an initial seed unique for this actor but deterministic with the dungeon's seed.
UCLASS(BlueprintType, Blueprintable, ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API UDeterministicRandomComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeterministicRandomComponent();

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	//~ End UActorComponent Interface

	// This is the C++ accessor.
	const FRandomStream& GetRandom() const { return Random; }

private:
	UPROPERTY(BlueprintReadOnly, Category = "Deterministic Random", meta = (AllowPrivateAccess = true))
	FRandomStream Random;
};
