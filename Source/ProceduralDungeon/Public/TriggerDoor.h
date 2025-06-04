// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "TriggerDoor.generated.h"

// Door that opens when an actor enters the trigger box.
// You can customize which actors can open the door by overriding the IsValidActor function.
UCLASS()
class PROCEDURALDUNGEON_API ATriggerDoor : public ADoor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Door Trigger")
	class UBoxComponent* BoxComponent;

	UPROPERTY(Transient)
	TSet<class AActor*> ActorList;

public:
	ATriggerDoor();
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Should returns true if the actor can open the door.
	// Component is the component on the actor that triggered the door.
	// By default the actor is valid if it's a Character.
	UFUNCTION(BlueprintNativeEvent, Category = "Door")
	bool IsValidActor(AActor* Actor, UPrimitiveComponent* Component);

	FORCEINLINE void UpdateOpenState() { Open(ActorList.Num() > 0); }
};
