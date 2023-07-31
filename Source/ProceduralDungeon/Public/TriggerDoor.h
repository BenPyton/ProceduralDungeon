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

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "TriggerDoor.generated.h"

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
