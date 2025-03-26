/*
 * Copyright (c) 2023-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "ReplicableObject.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Templates/SubclassOf.h"
#include "RoomCustomData.generated.h"

class UActorComponent;

// Base class for user custom data embedded in room instances
UCLASS(Abstract, BlueprintType, Blueprintable)
class PROCEDURALDUNGEON_API URoomCustomData : public UReplicableObject, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	void CreateLevelComponent(class ARoomLevel* LevelActor);

	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override;
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PreSaveDungeon_Implementation() override;
	virtual void PostLoadDungeon_Implementation() override;
	//~ End IDungeonSaveInterface Interface

private:
	// Component to create and attach on the Level Script Actor of the instanced room.
	UPROPERTY(EditAnywhere, Category = "Dungeon Rules", meta = (AllowAbstract = false, AllowPrivateAccess = true))
	TSubclassOf<UActorComponent> LevelComponent {nullptr};

	// Keep a reference to the created component instance
	UPROPERTY(Transient)
	TWeakObjectPtr<UActorComponent> LevelComponentInstance {nullptr};

private:
	struct FSaveData
	{
		TArray<uint8> ComponentData;
	};

	TUniquePtr<FSaveData> SavedData;
};
