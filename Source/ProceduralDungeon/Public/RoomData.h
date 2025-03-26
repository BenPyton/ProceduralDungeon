/*
 * Copyright (c) 2019-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralDungeonTypes.h"
#include "Misc/EngineVersionComparison.h"
#include "VoxelBounds/VoxelBounds.h"
#include "RoomData.generated.h"

#if UE_VERSION_OLDER_THAN(5, 3, 0)
#define USE_LEGACY_DATA_VALIDATION 1
#else
#define USE_LEGACY_DATA_VALIDATION 0
#endif

class URoom;
class UDungeonGraph;
class URoomCustomData;
class UDoorType;

#if WITH_EDITOR
class URoomData;
DECLARE_MULTICAST_DELEGATE_OneParam(FRoomDataEditorEvent, URoomData*)
#endif

// Static data shared by multiple room instances.
// This data is used to define the room's properties and the room level to spawn.
UCLASS()
class PROCEDURALDUNGEON_API URoomData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category = "Level")
	TSoftObjectPtr<UWorld> Level {nullptr};

public:
	// This will force a random door to be chosen during the dungeon generation.
	// DEPRECATED: It will be removed in a future version of the plugin. As a replacement, you should return -1 as DoorIndex in the ChooseNextRoomData of your DungeonGenerator.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Doors")
	bool RandomDoor {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Doors")
	TArray<FDoorDef> Doors {FDoorDef()};

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector FirstPoint {0};

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector SecondPoint {1};

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	TSet<TSubclassOf<URoomCustomData>> CustomData;

public:
	URoomData();

	UFUNCTION(BlueprintPure, Category = "Room Data", meta = (DisplayName = "Door Count", CompactNodeTitle = "Door Count"))
	int GetNbDoor() const { return Doors.Num(); }

	UFUNCTION(BlueprintPure, Category = "Room Data")
	const FDoorDef& GetDoorDef(int32 DoorIndex) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasCompatibleDoor(const FDoorDef& DoorData) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	void GetCompatibleDoors(const FDoorDef& DoorData, TArray<int>& CompatibleDoors) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasDoorOfType(UDoorType* DoorType) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasAnyDoorOfType(const TArray<UDoorType*>& DoorTypes) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasAllDoorOfType(const TArray<UDoorType*>& DoorTypes) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasCustomData(TSubclassOf<URoomCustomData> CustomDataClass) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomDataList) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	bool HasAllCustomData(const TArray < TSubclassOf<URoomCustomData>>& CustomDataList) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Room Data")
	void InitializeRoom(URoom* Room, UDungeonGraph* Dungeon) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Room Data")
	void CleanupRoom(URoom* Room, UDungeonGraph* Dungeon) const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	FIntVector GetSize() const;

	UFUNCTION(BlueprintPure, Category = "Room Data")
	int GetVolume() const;

	class FBoxCenterAndExtent GetBounds(FTransform Transform = FTransform::Identity) const;
	FBoxMinAndMax GetIntBounds() const;
	FVoxelBounds GetVoxelBounds() const;

	bool IsRoomInBounds(const FBoxMinAndMax& Bounds, int DoorIndex, const FDoorDef& DoorDungeonPos) const;

#if !(UE_BUILD_SHIPPING) || WITH_EDITOR
	bool IsDoorValid(int DoorIndex) const;
	bool IsDoorDuplicate(int DoorIndex) const;
#endif // !(UE_BUILD_SHIPPING) || WITH_EDITOR

#if WITH_EDITOR

#if USE_LEGACY_DATA_VALIDATION
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#else
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // USE_LEGACY_DATA_VALIDATION

	FRoomDataEditorEvent OnPropertiesChanged;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif // WITH_EDITOR

private:
	// This is a transient to avoid unnecessary computation
	// Flagged as mutable because it is computed on the fly when necessary.
	// This flag will be removed when the VoxelBounds editor will be implemented.
	mutable FVoxelBounds CachedVoxelBounds;
};
