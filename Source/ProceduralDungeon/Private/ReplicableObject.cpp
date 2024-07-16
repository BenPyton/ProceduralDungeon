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
}

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
	AActor* Owner = GetOwner();
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

	if (Owner->IsReplicatedSubObjectRegistered(this) == bRegister)
	{
		ensureMsgf(false, TEXT("Trying to %sregister %s as replicable subobject in actor %s but it is already %sregistered."), ::GetWithPredicate(TEXT("un"), !bRegister), *GetNameSafe(this), *GetNameSafe(Owner), ::GetWithPredicate(TEXT("un"), !bRegister));
		return;
	}

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

AActor* UReplicableObject::GetOwner() const
{
	UObject* Outer = GetOuter();
	while (Outer && !Outer->IsA<AActor>())
		Outer = Outer->GetOuter();
	return Cast<AActor>(Outer);
}

FString UReplicableObject::GetAuthorityName() const
{
	AActor* OwnerActor = Cast<AActor>(GetOuter());
	if (!OwnerActor)
		return TEXT("INVALID");
	return OwnerActor->HasAuthority() ? TEXT("Server") : TEXT("Client");
}

void UReplicableObject::WakeUpOwnerActor()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
		return;

	if (!Owner->HasAuthority())
		return;

	Owner->FlushNetDormancy();
}
