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
