/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
	Destroy,		// Immediately destroy the subobject on remotes. It's the responsibility of the authority to destroy it later. Imply Unregister.
	TearOff,		// Break replication of this subobject, so it's considered as local subobject on remotes. Imply Unregister.
};

struct FRegisterSubObjectParams
{
	// The NetCondition when registering the subobject. Unused when unregistering.
	ELifetimeCondition NetCondition = COND_None;
	// How the subobject is unregistered. Unused when registering.
	EUnregisterSubObjectType UnregisterType = EUnregisterSubObjectType::Unregister;
};

// Base class for sub-objects that can be replicated.
// This class is not blueprintable and should not be used directly in blueprints.
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

	// Returns true if the owning actor has authority
	bool HasAuthority() const;

	// Returns "Server" or "Client" based on HasAuthority() result.
	FString GetAuthorityName() const;

	// Wakes up the owner actor from dormance to indicate that a replicated variable will change.
	// You have to use it *before* modifying the replicated variables.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Replication")
	void WakeUpOwnerActor();
};
