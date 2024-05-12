/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TriggerType.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTriggerEvent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTriggerArrayEvent, TArray<AActor*>, Actor);

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
