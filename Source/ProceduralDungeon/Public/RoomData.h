/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralDungeonTypes.h"
#include "Misc/EngineVersionComparison.h"
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
};
