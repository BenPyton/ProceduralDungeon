/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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
