/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
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
#include "Components/ActorComponent.h"
#include "ProceduralDungeonTypes.h"
#include "StaticRoomVisibilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomVisibilityEvent, AActor*, Actor, bool, IsInVisibleRoom);

UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent, DisplayName = "Room Visibility (Static)") )
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
	void RegisterVisibilityDelegate(ARoomLevel* RoomLevel,  bool Register);

	UFUNCTION()
	void RoomVisibilityChanged(class ARoomLevel* RoomLevel, bool IsVisible);

private:
	void CleanEnablers();

	TSet<TWeakObjectPtr<UObject>> VisibilityEnablers {};

	UPROPERTY(EditAnywhere, BlueprintGetter = GetVisibilityMode, BlueprintSetter = SetVisibilityMode, Category = "Procedural Dungeon", meta = (AllowPrivateAccess = true))
	EVisibilityMode VisibilityMode {EVisibilityMode::Default};
};
