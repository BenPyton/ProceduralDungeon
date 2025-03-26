/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "RoomCustomData.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "RoomLevel.h"
#include "Utils/DungeonSaveUtils.h"

UActorComponent* CreateComponentOnInstance(AActor* ActorInstance, TSubclassOf<UActorComponent> ComponentClass, USceneComponent* OptionalParentForSceneComponent = nullptr)
{
	if (!ActorInstance || !ComponentClass)
		return nullptr;

	// Don't create component if on a template actor (CDO or Archetype)
	if (ActorInstance->IsTemplate())
		return nullptr;

	// For multiplayer games, create component only on server if component is replicating
	const UActorComponent* ComponentCDO = ComponentClass->GetDefaultObject<UActorComponent>();
	if (ComponentCDO->GetIsReplicated() && !ActorInstance->HasAuthority())
		return nullptr;

	UActorComponent* NewComp = NewObject<UActorComponent>(ActorInstance, ComponentClass);

	// Handles USceneComponent attachment
	if (USceneComponent* NewSceneComp = Cast<USceneComponent>(NewComp))
	{
		USceneComponent* ParentComponent = OptionalParentForSceneComponent;
		if (!ParentComponent)
			ParentComponent = ActorInstance->GetRootComponent();

		if (ParentComponent)
		{
			// Parent component should always be owned by the passed in actor instance!
			check(ParentComponent->GetOwner() != ActorInstance);
			NewSceneComp->SetupAttachment(ParentComponent);
		}
		else
		{
			// Set component directly as root if no root component on the actor
			ActorInstance->SetRootComponent(NewSceneComp);
		}
	}

	NewComp->RegisterComponent();
	return NewComp;
}

void URoomCustomData::CreateLevelComponent(ARoomLevel* LevelActor)
{
	if (!LevelComponent)
		return;

	LevelComponentInstance = CreateComponentOnInstance(LevelActor, LevelComponent);
	if (!LevelComponentInstance.IsValid())
	{
		DungeonLog_Error("Failed to create component '%s' on room level '%s'.", *GetNameSafe(LevelComponent), *GetNameSafe(LevelActor));
	}
}

bool URoomCustomData::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
{
	// Nothing more to serialize if no component
	if (nullptr == LevelComponent)
		return true;

	SavedData = MakeUnique<FSaveData>();

	if (!bIsLoading)
	{
		// Serialize component data
		if (LevelComponentInstance.IsValid())
		{
			SerializeUObject(SavedData->ComponentData, LevelComponentInstance.Get(), false);
		}
	}

	Record.EnterField(AR_FIELD_NAME("ComponentData")) << SavedData->ComponentData;

	if (!bIsLoading)
	{
		// No need to keep the data after saving
		SavedData.Reset();
	}

	return true;
}

void URoomCustomData::PreSaveDungeon_Implementation()
{
	if (!LevelComponentInstance.IsValid())
		return;

	if (LevelComponentInstance->Implements<UDungeonSaveInterface>())
	{
		IDungeonSaveInterface::Execute_PreSaveDungeon(LevelComponentInstance.Get());
	}
}

void URoomCustomData::PostLoadDungeon_Implementation()
{
	if (!SavedData.IsValid())
		return;

	// Deserialize component data
	if (LevelComponentInstance.IsValid())
	{
		SerializeUObject(SavedData->ComponentData, LevelComponentInstance.Get(), true);
	}
	else
	{
		DungeonLog_Error("Failed to deserialize component data for '%s' in room custom data '%s'", *GetNameSafe(LevelComponent), *GetNameSafe(this));
	}

	SavedData.Reset();

	if (LevelComponentInstance.IsValid() && LevelComponentInstance->Implements<UDungeonSaveInterface>())
	{
		IDungeonSaveInterface::Execute_PostLoadDungeon(LevelComponentInstance.Get());
	}
}
