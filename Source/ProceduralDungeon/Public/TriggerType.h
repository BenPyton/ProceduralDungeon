// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TriggerType.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTriggerEvent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTriggerArrayEvent, TArray<AActor*>, Actor);

// Trigger box that manages detection of actors entering and exiting the trigger.
// Can execute gameplay logic at regular intervals, with an optional delay before activation.
// You can customize the actor type that can activate the trigger by setting the ActorType property.
UCLASS(BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, ScriptName = "ActorTrigger"))
class PROCEDURALDUNGEON_API UTriggerType : public UBoxComponent
{
	GENERATED_BODY()

public:
	UTriggerType();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Type")
	float TickDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Type")
	float ActivationDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Type")
	uint8 requiredActorCountToActivate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Type")
	TSubclassOf<AActor> ActorType;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Actor Enter"))
	FActorTriggerEvent OnActorEnter;
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Actor Exit"))
	FActorTriggerEvent OnActorExit;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Tick"))
	FActorTriggerArrayEvent OnTriggerTick;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Activation"))
	FActorTriggerArrayEvent OnActivation;
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Deactivation"))
	FActorTriggerArrayEvent OnDeactivation;

	UFUNCTION()
	bool IsActivated() { return bIsActivated; }

	UFUNCTION()
	TArray<AActor*> GetActorList() { return ActorList; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger Type")
	bool bIsActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger Type")
	TArray<AActor*> ActorList;

private:
	UPROPERTY()
	FTimerHandle TickTimer;

	UPROPERTY()
	FTimerHandle ActivationTimer;

	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TriggerTick();

	UFUNCTION()
	void TriggerActivate();

	UFUNCTION()
	void TriggerDeactivate();
};
