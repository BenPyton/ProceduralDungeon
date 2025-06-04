// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

// Level script actor for a room instance.
// This class is used to manage the room instance and its bounds.
UCLASS(Blueprintable, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ARoomLevel : public ALevelScriptActor
{
	GENERATED_BODY()

	// TODO: Make those properties protected with public getter instead
public:
	UPROPERTY(EditAnywhere, Category = "Data")
	class URoomData* Data;

	UPROPERTY() // Macro is here to mark property to be used by GC
	URoom* Room = nullptr;

#if WITH_EDITOR
	// Flag to know when the Dungeon Editor mode is active.
	static bool bIsDungeonEditorMode;
#endif

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
