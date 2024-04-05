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
#include "Engine/LevelScriptActor.h"
#include "Math/GenericOctree.h" // for FBoxCenterAndExtent (required for UE5.0)
#include "RoomLevel.generated.h"

class ARoomLevel;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomLevelVisibilityEvent, ARoomLevel*, RoomLevel, bool, IsVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomLevelActorEvent, ARoomLevel*, RoomLevel, AActor*, Actor);

#if WITH_EDITOR
DECLARE_MULTICAST_DELEGATE_OneParam(FRoomLevelEditorEvent, ARoomLevel*);
#endif

class URoom;

UCLASS(Blueprintable, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ARoomLevel : public ALevelScriptActor
{
	GENERATED_BODY()

	// TODO: Make those properties protected is public getter instead
public:
	UPROPERTY(EditAnywhere, Category = "Data")
	class URoomData* Data;

	UPROPERTY() // Macro is here to mark property to be used by GC
	URoom* Room = nullptr;

public:
	ARoomLevel(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	void Init(URoom* Room);
	void SetActorsVisible(bool Visible);

	FORCEINLINE bool IsInit() const { return bIsInit; }

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Player Inside", DeprecatedFunction, DeprecationMessage = "Use GetRoom() instead to access directly the room functions."))
	bool IsPlayerInside();

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Visible", DeprecatedFunction, DeprecationMessage = "Use GetRoom() instead to access directly the room functions."))
	bool IsVisible();

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is Locked", DeprecatedFunction, DeprecationMessage = "Use GetRoom() instead to access directly the room functions."))
	bool IsLocked();

	UFUNCTION(BlueprintCallable, Category = "Procedural Dungeon", meta = (DeprecatedFunction, DeprecationMessage = "Use GetRoom() instead to access directly the room functions."))
	void Lock(bool lock);

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Room"))
	URoom* GetRoom() { return Room; }

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon")
	FVector GetBoundsCenter() const;

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon")
	FVector GetBoundsExtent() const;

public:
	// Event to notify when the visibility of the room has been toggled.
	UPROPERTY(BlueprintAssignable, Category = "Procedural Dungeon")
	FRoomLevelVisibilityEvent VisibilityChangedEvent;

	// Called when an actor enters the room bounds.
	UPROPERTY(BlueprintAssignable, Category = "Procedural Dungeon")
	FRoomLevelActorEvent ActorEnterRoomEvent;

	// Called when an actor exits the room bounds.
	UPROPERTY(BlueprintAssignable, Category = "Procedural Dungeon")
	FRoomLevelActorEvent ActorExitRoomEvent;

private:
	bool bIsInit = false;
	FTransform DungeonTransform;
	FBoxCenterAndExtent Bounds;
	UPROPERTY(Transient)
	class UBoxComponent* RoomTrigger = nullptr;
	TSet<TWeakObjectPtr<UObject>> Visitors;

private:
	void UpdateBounds();
	void UpdateVisitor(UObject* Visitor, bool IsInside);
	void TriggerActor(AActor* Actor, bool IsInTrigger);
	virtual void PostInitProperties() override;

#if WITH_EDITOR
public:
	FRoomLevelEditorEvent OnPropertiesChanged;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
