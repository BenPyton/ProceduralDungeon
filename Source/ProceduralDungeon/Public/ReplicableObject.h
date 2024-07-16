/*
 * MIT License
 *
 * Copyright (c) 2023-2024 Benoit Pelletier
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
#include "UObject/CoreNetTypes.h"
#include "UObject/NoExportTypes.h"
#include "Utils/ReplicationDefines.h"
#include "ReplicableObject.generated.h"

class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

enum class EUnregisterSubObjectType : uint8
{
	Unregister = 0, // Simply remove from the subobject list. Subobject on remotes will be destroyed when it's garbage collected on authority.
	Destroy, // Immediately destroy the subobject on remotes. It's the responsibility of the authority to destroy it later. Imply Unregister.
	TearOff, // Break replication of this subobject, so it's considered as local subobject on remotes. Imply Unregister.
};

struct FRegisterSubObjectParams
{
	// The NetCondition when registering the subobject. Unused when unregistering.
	ELifetimeCondition NetCondition = COND_None;
	// How the subobject is unregistered. Unused when registering.
	EUnregisterSubObjectType UnregisterType = EUnregisterSubObjectType::Unregister;
};

UCLASS(NotBlueprintable, NotBlueprintType)
class PROCEDURALDUNGEON_API UReplicableObject : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface

#if WITH_ENGINE
	virtual UWorld* GetWorld() const override;
#endif

	virtual bool IsSupportedForNetworking() const override { return true; }

#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	//~ End UObject Interface

	AActor* GetOwner() const;

	// To be called in place of Channel->ReplicateSubobject(...)
	// Considered as "Legacy" (used when "Use Registered Subobject List" is false in the actor owner).
	bool ReplicateSubobject(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	// Call AddReplicatedSubObject to its owner (if valid).
	// Call it during PostInitializeComponent if subobject was created using CreateDefaultSubobject.
	// Used when "Use Registered Subobject List" is true in the actor owner.
	void RegisterAsReplicable(bool bRegister, FRegisterSubObjectParams Params = FRegisterSubObjectParams());

protected:
	// Replicating the eventual nested subobjects.
	// Should not be called directly, except from the overridden function in child classes.
	// Considered as "Legacy" (used when "Use Registered Subobject List" is false in the actor owner).
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	// Override this to register subobjects of this one as replicable.
	// Used when "Use Registered Subobject List" is true in the actor owner.
	virtual void RegisterReplicableSubobjects(bool bRegister) {}

	// Returns "Server" or "Client" based on HasAuthority() result.
	FString GetAuthorityName() const;

	// Wakes up the owner actor from dormance to indicate that a replicated variable will change.
	// You have to use it *before* modifying the replicated variables.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Replication")
	void WakeUpOwnerActor();
};
