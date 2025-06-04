// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

	static int32 GenerateDeterministicSeed(AActor* Actor);

	// This is the C++ accessor.
	const FRandomStream& GetRandom() const { return Random; }

private:
	UPROPERTY(BlueprintReadOnly, Category = "Deterministic Random", meta = (AllowPrivateAccess = true))
	FRandomStream Random;
};
