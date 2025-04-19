/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "ReplicableObject.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonLog.h"
#include "Utils/ReplicationUtils.h"

namespace
{
	const TCHAR* GetWithPredicate(const TCHAR* Str, bool bPredicate)
	{
		return (bPredicate) ? Str : TEXT("");
	}
} //namespace

bool UReplicableObject::ReplicateSubobject(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	// Ensure that nested objects are replicated BEFORE!
	// thus any reference to them inside this object will be correct.
	bool bWroteSomething = ReplicateSubobjects(Channel, Bunch, RepFlags);
	bWroteSomething |= Channel->ReplicateSubobject(this, *Bunch, *RepFlags);
	return bWroteSomething;
}

void UReplicableObject::RegisterAsReplicable(bool bRegister, FRegisterSubObjectParams Params)
{
#if UE_WITH_SUBOBJECT_LIST
	AActor* Owner = GetTypedOuter<AActor>();
	if (!IsValid(Owner))
	{
		ensureMsgf(false, TEXT("Trying to %sregister %s as replicable subobject but actor owner is invalid."), ::GetWithPredicate(TEXT("un"), !bRegister), *GetNameSafe(this));
		return;
	}

	if (!Owner->HasAuthority())
		return;

	// Ignores if owner does not use registered subobject list
	if (!Owner->IsUsingRegisteredSubObjectList())
		return;

	// Ignore if the object is already registered/unregistered
	if (Owner->IsReplicatedSubObjectRegistered(this) == bRegister)
		return;

	DungeonLog_InfoSilent("%s Replicable Subobject: %s", (bRegister) ? TEXT("Register") : TEXT("Unregister"), *GetNameSafe(this));

	if (bRegister)
		Owner->AddReplicatedSubObject(this, Params.NetCondition);
	else
	{
		switch (Params.UnregisterType)
		{
		case EUnregisterSubObjectType::Unregister:
			Owner->RemoveReplicatedSubObject(this);
			break;
	#if UE_VERSION_NEWER_THAN(5, 2, 0)
		case EUnregisterSubObjectType::Destroy:
			Owner->DestroyReplicatedSubObjectOnRemotePeers(this);
			break;
		case EUnregisterSubObjectType::TearOff:
			Owner->TearOffReplicatedSubObjectOnRemotePeers(this);
			break;
	#endif
		default:
			checkf(false, TEXT("Unimplemented case."));
			break;
		}
	}

	RegisterReplicableSubobjects(bRegister);
#endif
}

#if UE_WITH_IRIS
void UReplicableObject::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	// Build descriptors and allocate PropertyReplicationFragments for this object
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

bool UReplicableObject::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return false;
}

UWorld* UReplicableObject::GetWorld() const
{
	UObject* Outer = GetOuter();
	if (!Outer)
		return nullptr;
	return Outer->GetWorld();
}

bool UReplicableObject::HasAuthority() const
{
	AActor* Owner = GetTypedOuter<AActor>();
	if (!Owner)
		return false;
	return Owner->HasAuthority();
}

FString UReplicableObject::GetAuthorityName() const
{
	return HasAuthority() ? TEXT("Server") : TEXT("Client");
}

void UReplicableObject::WakeUpOwnerActor()
{
	AActor* Owner = GetTypedOuter<AActor>();
	if (!IsValid(Owner))
		return;

	if (!Owner->HasAuthority())
		return;

	Owner->FlushNetDormancy();
}
