/*
 * MIT License
 *
 * Copyright (c) 2023-2024 Benoit Pelletier
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

#include "RoomVisibilityComponent.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "RoomLevel.h"

URoomVisibilityComponent::URoomVisibilityComponent()
	: VisibilityMode(EVisibilityMode::Default)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisibility();
}

void URoomVisibilityComponent::OnRoomEnter_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_InfoSilent("Visibility Component '%s' Enters Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	if (!IsValid(RoomLevel))
		return;

	SetVisible(RoomLevel, RoomLevel->IsVisible());
	RoomLevel->VisibilityChangedEvent.AddDynamic(this, &URoomVisibilityComponent::RoomVisibilityChanged);
}

void URoomVisibilityComponent::OnRoomExit_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_InfoSilent("Visibility Component '%s' Exits Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	if (!IsValid(RoomLevel))
		return;

	RoomLevel->VisibilityChangedEvent.RemoveDynamic(this, &URoomVisibilityComponent::RoomVisibilityChanged);
	SetVisible(RoomLevel, false);
}

bool URoomVisibilityComponent::IsVisible()
{
	return Dungeon::OccludeDynamicActors() ? VisibilityEnablers.Num() > 0: true;
}

void URoomVisibilityComponent::SetVisible(UObject* Owner, bool Visible)
{
	const bool bOldVisible = IsVisible();
	if (Visible)
		VisibilityEnablers.Add(Owner);
	else
		VisibilityEnablers.Remove(Owner);

	const bool bNewVisible = IsVisible();
	DungeonLog_InfoSilent("Visibility of '%s' Changed: %d (before: %d)", *GetNameSafe(GetOwner()), bNewVisible, bOldVisible);
	if (bOldVisible != bNewVisible)
	{
		UpdateVisibility();
		DungeonLog_InfoSilent("Dispatch Room Visibility Event of '%s'", *GetNameSafe(GetOwner()));
		OnRoomVisibilityChanged.Broadcast(GetOwner(), bNewVisible);
	}
}

void URoomVisibilityComponent::ResetVisible(UObject* Owner)
{
	SetVisible(Owner, false);
}

void URoomVisibilityComponent::SetVisibilityMode(EVisibilityMode Mode)
{
	VisibilityMode = Mode;
	UpdateVisibility();
}

void URoomVisibilityComponent::UpdateVisibility()
{
	CleanEnablers();

	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		// Can't use Actor->SetActorHiddenInGame() because it is replicated over network.
		USceneComponent* Root = Actor->GetRootComponent();
		if (IsValid(Root))
		{
			switch (VisibilityMode)
			{
			case EVisibilityMode::Default:
				Root->SetVisibility(IsVisible(), true);
				break;
			case EVisibilityMode::ForceHidden:
				Root->SetVisibility(false, true);
				break;
			case EVisibilityMode::ForceVisible:
				Root->SetVisibility(true, true);
				break;
			case EVisibilityMode::Custom:
				// The user handles the visibility
				break;
			default:
				checkNoEntry();
				break;
			}
		}
	}
}

void URoomVisibilityComponent::RoomVisibilityChanged(ARoomLevel* RoomLevel, bool IsVisible)
{
	DungeonLog_InfoSilent("[%s] Room '%s' Visibility Changed: %d", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel), IsVisible);
	SetVisible(RoomLevel, IsVisible);
}

void URoomVisibilityComponent::CleanEnablers()
{
	TSet<TWeakObjectPtr<UObject>> ObjPtrToRemove;
	for (TWeakObjectPtr<UObject> ObjPtr : VisibilityEnablers)
	{
		if (!ObjPtr.IsValid())
			ObjPtrToRemove.Add(ObjPtr);
	}

	for (TWeakObjectPtr<UObject> ObjPtr : ObjPtrToRemove)
	{
		VisibilityEnablers.Remove(ObjPtr);
	}
}
