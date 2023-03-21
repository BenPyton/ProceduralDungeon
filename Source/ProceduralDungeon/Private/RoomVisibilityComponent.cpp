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

#include "RoomVisibilityComponent.h"
#include "Room.h"

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

bool URoomVisibilityComponent::IsVisible()
{
	return URoom::OccludeDynamicActors() ? VisibilityEnablers.Num() > 0: true;
}

void URoomVisibilityComponent::SetVisible(UObject* Owner, bool Visible)
{
	const bool bOldVisible = IsVisible();
	if (Visible)
		VisibilityEnablers.Add(Owner);
	else
		VisibilityEnablers.Remove(Owner);

	const bool bNewVisible = IsVisible();
	if (bOldVisible != bNewVisible)
	{
		UpdateVisibility();
		OnRoomVisibilityChanged.Broadcast(GetOwner(), bNewVisible);
	}
}

void URoomVisibilityComponent::ResetVisible(UObject* Owner)
{
	const bool bOldVisible = IsVisible();
	VisibilityEnablers.Remove(Owner);

	const bool bNewVisible = IsVisible();
	if (bOldVisible != bNewVisible)
	{
		UpdateVisibility();
		OnRoomVisibilityChanged.Broadcast(GetOwner(), bNewVisible);
	}
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