// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TriggerType.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerEvent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerArrayEvent, TArray<AActor*>, Actor);


UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALDUNGEON_API UTriggerType : public UBoxComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTriggerType();

protected:
	// Called when the game starts
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
	FTriggerEvent OnActorEnter;
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Actor Exit"))
	FTriggerEvent OnActorExit;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Tick"))
	FTriggerArrayEvent OnTriggerTick;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Activation"))
	FTriggerArrayEvent OnActivation;
	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Trigger Deactivation"))
	FTriggerArrayEvent OnDeactivation;

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
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TriggerTick();

	UFUNCTION()
	void TriggerActivate();

	UFUNCTION()
	void TriggerDeactivate();
};
