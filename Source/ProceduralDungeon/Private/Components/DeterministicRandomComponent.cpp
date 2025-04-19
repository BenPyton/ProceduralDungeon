/*
 * Copyright (c) 2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
} //namespace

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
