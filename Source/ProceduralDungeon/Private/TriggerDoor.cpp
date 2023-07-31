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

#include "TriggerDoor.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Room.h"
#include "RoomLevel.h"

ATriggerDoor::ATriggerDoor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	if (IsValid(RootComponent))
		BoxComponent->SetupAttachment(RootComponent);
}

void ATriggerDoor::BeginPlay()
{
	Super::BeginPlay();

	check(BoxComponent);
	BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATriggerDoor::OnTriggerEnter);
	BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ATriggerDoor::OnTriggerExit);
}

void ATriggerDoor::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ActorList.Contains(OtherActor) && IsValidActor(OtherActor, OtherComp))
	{
		ActorList.Add(OtherActor);
		UpdateOpenState();
	}
}

void ATriggerDoor::OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ActorList.Contains(OtherActor))
	{
		ActorList.Remove(OtherActor);
		UpdateOpenState();
	}
}

bool ATriggerDoor::IsValidActor_Implementation(AActor* Actor, UPrimitiveComponent* Component)
{
	ACharacter* Character = Cast<ACharacter>(Actor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Component);
	return IsValid(Character) && IsValid(Capsule) && Capsule == Character->GetCapsuleComponent();
}
