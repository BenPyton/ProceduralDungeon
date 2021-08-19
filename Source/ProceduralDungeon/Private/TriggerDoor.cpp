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
#include "GameFramework/Character.h"
#include "Room.h"
#include "RoomLevel.h"

ATriggerDoor::ATriggerDoor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	if (RootComponent != nullptr)
	{
		BoxComponent->SetupAttachment(RootComponent);
	}
}

void ATriggerDoor::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATriggerDoor::OnTriggerEnter);
		BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ATriggerDoor::OnTriggerExit);
	}
}

void ATriggerDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CharacterList.Num() > 0)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}

void ATriggerDoor::SetRoomsAlwaysVisible(bool _visible)
{
	if (nullptr != RoomA && nullptr != RoomA->GetLevelScript())
	{
		RoomA->GetLevelScript()->AlwaysVisible = _visible;
	}
	if (nullptr != RoomB && nullptr != RoomB->GetLevelScript())
	{
		RoomB->GetLevelScript()->AlwaysVisible = _visible;
	}
}

void ATriggerDoor::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	UCapsuleComponent* OtherCapsule = Cast<UCapsuleComponent>(OtherComp);

	if (OtherCharacter != nullptr && OtherCapsule != nullptr && OtherCapsule == OtherCharacter->GetCapsuleComponent() && !CharacterList.Contains(OtherCharacter))
	{
		CharacterList.Add(OtherCharacter);
	}

}

void ATriggerDoor::OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	UCapsuleComponent* OtherCapsule = Cast<UCapsuleComponent>(OtherComp);

	if (OtherCharacter != nullptr && OtherCapsule != nullptr && OtherCapsule == OtherCharacter->GetCapsuleComponent() && CharacterList.Contains(OtherCharacter))
	{
		CharacterList.Remove(OtherCharacter);
	}
}
