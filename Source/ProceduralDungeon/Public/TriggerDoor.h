// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "TriggerDoor.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALDUNGEON_API ATriggerDoor : public ADoor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="Door Trigger")
	class UBoxComponent* BoxComponent;

	UPROPERTY()
	TArray<class ACharacter*> CharacterList;

public:
	ATriggerDoor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetRoomsAlwaysVisible(bool Visible);
};
