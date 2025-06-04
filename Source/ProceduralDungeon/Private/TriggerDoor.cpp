// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
