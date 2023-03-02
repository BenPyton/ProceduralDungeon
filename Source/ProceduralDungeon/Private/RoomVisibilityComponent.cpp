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

URoomVisibilityComponent::URoomVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	VisibilityMode = EVisibilityMode::Default;
}

bool URoomVisibilityComponent::IsVisible()
{
	return VisibilityEnablers.Num() > 0;
}

void URoomVisibilityComponent::SetVisible(UObject* Owner, bool Visible)
{
	if (Visible)
		VisibilityEnablers.Add(Owner);
	else
		VisibilityEnablers.Remove(Owner);

	UpdateVisibility();
}

void URoomVisibilityComponent::ResetVisible(UObject* Owner)
{
	VisibilityEnablers.Remove(Owner);
	UpdateVisibility();
}

void URoomVisibilityComponent::SetVisibilityMode(EVisibilityMode Mode)
{
	VisibilityMode = Mode;
	UpdateVisibility();
}

void URoomVisibilityComponent::UpdateVisibility()
{
	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		switch (VisibilityMode)
		{
		case EVisibilityMode::Default:
			Actor->SetActorHiddenInGame(!IsVisible());
			break;
		case EVisibilityMode::ForceHidden:
			Actor->SetActorHiddenInGame(true);
			break;
		case EVisibilityMode::ForceVisible:
			Actor->SetActorHiddenInGame(false);
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