#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/LevelStreaming.h"
#include "ProceduralLevelStreaming.generated.h"

/**
 * This file was copied from the ULevelStreamingDynamic of the engine
 * But an Unload methods is added and the UniqueLevelInstanceID is replaced by a string suffix passed in parameter of the different load funtcions
 */
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UProceduralLevelStreaming : public ULevelStreaming
{
	GENERATED_UCLASS_BODY()

	/** Whether the level should be loaded at startup																			*/
	UPROPERTY(Category = LevelStreaming, EditAnywhere)
	uint32 bInitiallyLoaded : 1;

	/** Whether the level should be visible at startup if it is loaded 															*/
	UPROPERTY(Category = LevelStreaming, EditAnywhere)
	uint32 bInitiallyVisible : 1;

	/**
	* Stream in a level with a specific location and rotation. You can create multiple instances of the same level!
	*
	* The level to be loaded does not have to be in the persistent map's Levels list, however to ensure that the .umap does get
	* packaged, please be sure to include the .umap in your Packaging Settings:
	*
	*   Project Settings -> Packaging -> List of Maps to Include in a Packaged Build (you may have to show advanced or type in filter)
	*
	* @param LevelName - Level package name, ex: /Game/Maps/MyMapName, specifying short name like MyMapName will force very slow search on disk
	* @param Location - World space location where the level should be spawned
	* @param Rotation - World space rotation for rotating the entire level
	* @param bOutSuccess - Whether operation was successful (map was found and added to the sub-levels list)
	* @return Streaming level object for a level instance
	*/
	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Name)", WorldContext = "WorldContextObject"))
	static UProceduralLevelStreaming* LoadLevelInstance(UObject* WorldContextObject, FString LevelName, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation, bool& bOutSuccess);

	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Object Reference)", WorldContext = "WorldContextObject"))
	static UProceduralLevelStreaming* LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation, bool& bOutSuccess);



	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Room Data)", WorldContext = "WorldContextObject"))
	static UProceduralLevelStreaming* Load(UObject* WorldContextObject, class URoomData* Data, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Unload Level Instance", WorldContext = "WorldContextObject"))
	static void Unload(UObject* WorldContextObject, UProceduralLevelStreaming* Instance);


	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin ULevelStreaming Interface
	virtual bool ShouldBeLoaded() const override { return bShouldBeLoaded; }
	virtual bool ShouldBeAlwaysLoaded() const override { return true; }
	//~ End ULevelStreaming Interface

	virtual void SetShouldBeLoaded(bool bShouldBeLoaded) override;

	UFUNCTION(BlueprintCallable, Category = "Procedural Level Streaming")
	void OnLevelDynamicUnloaded();

	bool IsLevelUnloaded() { return bIsUnloaded; }

private:

	uint32 bIsUnloaded : 1;

	static UProceduralLevelStreaming* LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation, bool& bOutSuccess);
};
