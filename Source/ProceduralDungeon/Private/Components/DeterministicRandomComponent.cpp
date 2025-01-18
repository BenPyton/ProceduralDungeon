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

#include "Components/DeterministicRandomComponent.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonlog.h"
#include "DungeonBlueprintLibrary.h"
#include "Room.h"
#include "Interfaces/RoomActorGuid.h"
#include "DungeonGeneratorBase.h"

namespace
{
	// Utility functions to xor 2 guids
	FGuid operator^(const FGuid& A, const FGuid& B)
	{
		return FGuid(A.A ^ B.A, A.B ^ B.B, A.C ^ B.C, A.D ^ B.D);
	}

	void operator^=(FGuid& A, const FGuid& B)
	{
		A = A ^ B;
	}
}

UDeterministicRandomComponent::UDeterministicRandomComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDeterministicRandomComponent::OnRegister()
{
	Super::OnRegister();

	AActor* OwnerActor = GetOwner();
	const int32 Seed = GenerateDeterministicSeed(OwnerActor);
	Random.Initialize(Seed);
	DungeonLog_Debug("[%s] Initial seed set to: %d.", *GetNameSafe(OwnerActor), Random.GetInitialSeed());
}

// *WARNING*: Updating the algorithm may break systems relying on this generated seed.
// The best approach is to not rely on this generated seed.
int32 UDeterministicRandomComponent::GenerateDeterministicSeed(AActor* Actor)
{
	FGuid Guid;
	int64 Salt = 0;

	// Use the guid of the actor if available.
	UObject* GuidImplementer = IRoomActorGuid::GetImplementer(Actor);
	if (IsValid(GuidImplementer))
	{
		Guid = IRoomActorGuid::Execute_GetGuid(GuidImplementer);
	}

	// Get the room ID as the salt if available.
	if (const URoom* OwningRoom = UDungeonBlueprintLibrary::GetOwningRoom(Actor))
	{
		Salt = OwningRoom->GetRoomID();

		// Will also use the generator guid and seed if relevant.
		if (const ADungeonGeneratorBase* Generator = OwningRoom->Generator())
		{
			Guid ^= Generator->GetGuid();

			const int64 Seed = static_cast<int64>(Generator->GetSeed());
			Salt ^= Seed << 32;
		}
	}

	return Random::Guid2Seed(Guid, Salt);
}
