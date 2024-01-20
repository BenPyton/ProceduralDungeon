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

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReplicableObject.generated.h"

class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

UCLASS(NotBlueprintable, NotBlueprintType)
class PROCEDURALDUNGEON_API UReplicableObject : public UObject
{
	GENERATED_BODY()

public:
	// Enable object for replication
	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual UWorld* GetWorld() const override;
	AActor* GetOwner() const;

	// To be called in place of Channel->ReplicateSubobject(...)
	bool ReplicateSubobject(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

protected:
	// Replicating the eventual nested subobjects.
	// Should not be called directly, except from the overridden function in child classes.
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	// Returns "Server" or "Client" based on HasAuthority() result.
	FString GetAuthorityName() const;

	// Wakes up the owner actor from dormance to indicate that a replicated variable will change.
	// You have to use it *before* modifying the replicated variables.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Replication")
	void WakeUpOwnerActor();
};
