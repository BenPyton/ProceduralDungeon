/*
 * Copyright (c) 2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "Components/StaticRoomVisibilityComponent.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "RoomLevel.h"

UStaticRoomVisibilityComponent::UStaticRoomVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStaticRoomVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisibility();
	RegisterVisibilityDelegate(GetOwnerRoomLevel(), true);
}

void UStaticRoomVisibilityComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	RegisterVisibilityDelegate(GetOwnerRoomLevel(), false);
}

bool UStaticRoomVisibilityComponent::IsVisible()
{
	return (Dungeon::OcclusionCulling() && Dungeon::OccludeDynamicActors()) ? VisibilityEnablers.Num() > 0 : true;
}

void UStaticRoomVisibilityComponent::SetVisible(UObject* Owner, bool Visible)
{
	const bool bOldVisible = IsVisible();
	if (Visible)
		VisibilityEnablers.Add(Owner);
	else
		VisibilityEnablers.Remove(Owner);

	const bool bNewVisible = IsVisible();
	DungeonLog_InfoSilent("Visibility of '%s' Changed: %d (before: %d)", *GetNameSafe(GetOwner()), bNewVisible, bOldVisible);
	if (bOldVisible == bNewVisible)
		return;

	UpdateVisibility();
	DungeonLog_InfoSilent("Dispatch Room Visibility Event of '%s'", *GetNameSafe(GetOwner()));
	OnRoomVisibilityChanged.Broadcast(GetOwner(), bNewVisible);
}

void UStaticRoomVisibilityComponent::ResetVisible(UObject* Owner)
{
	SetVisible(Owner, false);
}

void UStaticRoomVisibilityComponent::SetVisibilityMode(EVisibilityMode Mode)
{
	VisibilityMode = Mode;
	UpdateVisibility();
}

ARoomLevel* UStaticRoomVisibilityComponent::GetOwnerRoomLevel() const
{
	ULevel* Level = GetOwner()->GetLevel();
	if (!IsValid(Level))
		return nullptr;

	return Cast<ARoomLevel>(Level->GetLevelScriptActor());
}

void UStaticRoomVisibilityComponent::UpdateVisibility()
{
	CleanEnablers();

	AActor* Actor = GetOwner();
	if (!IsValid(Actor))
		return;

	// Can't use Actor->SetActorHiddenInGame() because it is replicated over network.
	// So instead we use the Root->SetVisibility() and propagate it to its children.
	// TODO: try to use something better than that (non-replicated but actor-wide).
	USceneComponent* Root = Actor->GetRootComponent();
	if (!IsValid(Root))
		return;

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

void UStaticRoomVisibilityComponent::RegisterVisibilityDelegate(ARoomLevel* RoomLevel, bool Register)
{
	if (!IsValid(RoomLevel))
		return;

	if (Register)
		RoomLevel->VisibilityChangedEvent.AddDynamic(this, &UStaticRoomVisibilityComponent::RoomVisibilityChanged);
	else
		RoomLevel->VisibilityChangedEvent.RemoveDynamic(this, &UStaticRoomVisibilityComponent::RoomVisibilityChanged);

	SetVisible(RoomLevel, RoomLevel->IsVisible());
}

void UStaticRoomVisibilityComponent::RoomVisibilityChanged(ARoomLevel* RoomLevel, bool IsVisible)
{
	DungeonLog_InfoSilent("[%s] Room '%s' Visibility Changed: %d", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel), IsVisible);
	SetVisible(RoomLevel, IsVisible);
}

void UStaticRoomVisibilityComponent::CleanEnablers()
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
