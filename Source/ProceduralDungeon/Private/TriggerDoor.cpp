// Fill out your copyright notice in the Description page of Project Settings.

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
