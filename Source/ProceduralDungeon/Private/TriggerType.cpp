/*
 * Copyright (c) 2019-2021 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "TriggerType.h"
#include "TimerManager.h"
#include "ProceduralDungeonTypes.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTriggerType::UTriggerType()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TickDuration = 0.5f;
	ActivationDelay = 0.0;
}


// Called when the game starts
void UTriggerType::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != ENetMode::NM_Client)
	{
		OnComponentBeginOverlap.AddUniqueDynamic(this, &UTriggerType::OnTriggerEnter);
		OnComponentEndOverlap.AddUniqueDynamic(this, &UTriggerType::OnTriggerExit);
		GetWorld()->GetTimerManager().SetTimer(TickTimer, this, &UTriggerType::TriggerTick, TickDuration, true);
	}
}

void UTriggerType::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorType == nullptr || (OtherActor != nullptr && OtherActor->IsA(ActorType)))
	{
		if (!ActorList.Contains(OtherActor))
		{
			ActorList.Add(OtherActor);
			OnActorEnter.Broadcast(OtherActor);

			if (ActorList.Num() >= requiredActorCountToActivate)
			{
				if (ActivationDelay > 0)
				{
					GetWorld()->GetTimerManager().SetTimer(ActivationTimer, this, &UTriggerType::TriggerActivate, ActivationDelay, false);
				}
				else
				{
					TriggerActivate();
				}
			}
		}
	}
}

void UTriggerType::OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ActorType == nullptr || (OtherActor != nullptr && OtherActor->IsA(ActorType)))
	{
		if (ActorList.Contains(OtherActor))
		{
			ActorList.Remove(OtherActor);
			OnActorExit.Broadcast(OtherActor);

			GetWorld()->GetTimerManager().ClearTimer(ActivationTimer);
			TriggerDeactivate();
		}
	}
}

void UTriggerType::TriggerTick()
{
	OnTriggerTick.Broadcast(ActorList);
}

void UTriggerType::TriggerActivate()
{
	if (!bIsActivated)
	{
		bIsActivated = true;
		OnActivation.Broadcast(ActorList);
	}
}

void UTriggerType::TriggerDeactivate()
{
	if (bIsActivated)
	{
		bIsActivated = false;
		OnDeactivation.Broadcast(ActorList);
	}
}
