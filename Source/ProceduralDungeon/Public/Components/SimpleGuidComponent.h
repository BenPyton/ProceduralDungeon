// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RoomActorGuid.h"
#include "SimpleGuidComponent.generated.h"

#define GUID_DEBUG_ENABLED 0

// A simple guid component that will retrieve the Editor's ActorGuid
// to save/load it in packaged games.
//
// :::warning
//
// This component will work only on placed actor, not actors spawned during runtime!!!
//
// :::
UCLASS(BlueprintType, Blueprintable, ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API USimpleGuidComponent : public UActorComponent, public IRoomActorGuid
{
	GENERATED_BODY()

public:
	USimpleGuidComponent();

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	//~ End UActorComponent Interface

	//~ Begin IRoomActorGuid Interface
	virtual FGuid GetGuid_Implementation() const override;
	virtual bool ShouldSaveActor_Implementation() const override;
	//~ End IRoomActorGuid Interface

	// Unfortunately I can't place them in the #if block below. UE will complain about it.
	virtual void Serialize(FArchive& Ar) override;
	virtual void Serialize(FStructuredArchive::FRecord Record) override;

#if GUID_DEBUG_ENABLED // Enable some logs to debug lifecycle of the component.
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void PreSave(FObjectPreSaveContext SaveContext);
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin UActorComponent Interface
	virtual void OnComponentCreated() override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface
#endif

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, Category = "Guid")
	FGuid Guid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeon Save")
	bool bSaveActorInDungeon {true};
};
