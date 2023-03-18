/*
* MIT License
*
* Copyright (c) 2023 Benoit Pelletier
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
#include "RoomVisibilityComponent.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "Room Visibility"))
enum class EVisibilityMode : uint8
{
	Default			UMETA(DisplayName = "Same As Room"),
	ForceVisible	UMETA(DisplayName = "Force Visible"),
	ForceHidden		UMETA(DisplayName = "Force Hidden"),
	Custom			UMETA(DisplayName = "Custom"),
	NbMode			UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoomVisibilityEvent, AActor*, Actor, bool, IsInVisibleRoom);

UCLASS(ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent, DisplayName = "Room Visibility"))
class PROCEDURALDUNGEON_API URoomVisibilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URoomVisibilityComponent();

	virtual void BeginPlay() override;

	void SetVisible(UObject* Owner, bool Visible);
	void ResetVisible(UObject* Owner);

	// Returns true if the actor is in a visible room.
	// Useful with "Custom" visibility.
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Is In Visible Room", DisplayName = "Is In Visible Room"))
	bool IsVisible();

	UFUNCTION(BlueprintCallable, Category = "Procedural Dungeon", meta = (DisplayName = "Set Visibility"))
	void SetVisibilityMode(EVisibilityMode Mode);

	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon", meta = (CompactNodeTitle = "Visibility", DisplayName = "Get Visibility"))
	FORCEINLINE EVisibilityMode GetVisibilityMode() { return VisibilityMode; }

	// Called when the visibility from rooms changed (either by a room visibility change or by this actor moving between rooms).
	// Useful to update a "Custom" visibility.
	UPROPERTY(BlueprintAssignable, Category = "Procedural Dungeon")
	FRoomVisibilityEvent OnRoomVisibilityChanged;

protected:
	void UpdateVisibility();

private:
	void CleanEnablers();

	TSet<TWeakObjectPtr<UObject>> VisibilityEnablers;
	EVisibilityMode VisibilityMode;
};
