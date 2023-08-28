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

bool UReplicableObject::ReplicateSubobject(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	// Ensure that nested objects are replicated BEFORE!
	// thus any reference to them inside this object will be correct.
	bool bWroteSomething = ReplicateSubobjects(Channel, Bunch, RepFlags);
	bWroteSomething |= Channel->ReplicateSubobject(this, *Bunch, *RepFlags);
	return bWroteSomething;
}

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

FString UReplicableObject::GetAuthorityName() const
{
	AActor* OwnerActor = Cast<AActor>(GetOuter());
	if (!OwnerActor)
		return TEXT("INVALID");
	return OwnerActor->HasAuthority() ? TEXT("Server") : TEXT("Client");
}
