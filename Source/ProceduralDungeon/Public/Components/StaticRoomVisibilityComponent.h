/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralDungeonTypes.h"
#include "StaticRoomVisibilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomVisibilityEvent, AActor*, Actor, bool, IsInVisibleRoom);

// Component to manage the visibility of an actor in the dungeon.
// Use this one if the actor remains in the same room.
// If the actor is able to move room, use URoomVisibilityComponent instead.
UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent, DisplayName = "Room Visibility (Static)"))
class PROCEDURALDUNGEON_API UStaticRoomVisibilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaticRoomVisibilityComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	void SetVisible(UObject* Owner, bool Visible);
	void ResetVisible(UObject* Owner); // Same as SetVisible(Owner, false)

	// Returns true if the actor is in a visible room.
	// Always returns true when "Occlude Dynamic Actors" is unchecked in the plugin's settings
	// Useful with "Custom" visibility.
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is In Visible Room", DisplayName = "Is In Visible Room"))
	bool IsVisible();

	UFUNCTION(BlueprintSetter)
	void SetVisibilityMode(EVisibilityMode Mode);

	UFUNCTION(BlueprintGetter)
	FORCEINLINE EVisibilityMode GetVisibilityMode() const { return VisibilityMode; }

public:
	// Called when the visibility from rooms changed (either by a room visibility change or by this actor moving between rooms).
	// Useful to update a "Custom" visibility.
	UPROPERTY(BlueprintAssignable, Category = "Procedural Dungeon")
	FRoomVisibilityEvent OnRoomVisibilityChanged;

protected:
	ARoomLevel* GetOwnerRoomLevel() const;
	void UpdateVisibility();
	void RegisterVisibilityDelegate(ARoomLevel* RoomLevel, bool Register);

	UFUNCTION()
	void RoomVisibilityChanged(class ARoomLevel* RoomLevel, bool IsVisible);

private:
	void CleanEnablers();

	TSet<TWeakObjectPtr<UObject>> VisibilityEnablers {};

	UPROPERTY(EditAnywhere, BlueprintGetter = GetVisibilityMode, BlueprintSetter = SetVisibilityMode, Category = "Procedural Dungeon", meta = (AllowPrivateAccess = true))
	EVisibilityMode VisibilityMode {EVisibilityMode::Default};
};
