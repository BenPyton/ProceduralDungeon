/*
 * MIT License
 *
 * Copyright (c) 2019-2021, 2023 Benoit Pelletier
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

#if WITH_EDITOR
class URoomData;
DECLARE_MULTICAST_DELEGATE_OneParam(FRoomDataEditorEvent, URoomData*)
#endif

UCLASS()
class PROCEDURALDUNGEON_API URoomData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class UProceduralLevelStreaming;

public:
	UPROPERTY(EditInstanceOnly, Category = "Level")
	TSoftObjectPtr<UWorld> Level {nullptr};

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Doors")
	bool RandomDoor {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Doors")
	TArray<FDoorDef> Doors {FDoorDef()};

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector FirstPoint {0};

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector SecondPoint {1};

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	TSet<TSubclassOf<class URoomCustomData>> CustomData;

public:
	URoomData();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Room Data", meta = (DisplayName = "Door Count", CompactNodeTitle = "Door Count"))
	int GetNbDoor() const { return Doors.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Room Data")
	bool HasCompatibleDoor(const FDoorDef& DoorData) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Room Data")
	void InitializeRoom(URoom* Room, UDungeonGraph* Dungeon) const;

	FIntVector GetSize() const;
	class FBoxCenterAndExtent GetBounds(FTransform Transform = FTransform::Identity) const;
	FBoxMinAndMax GetIntBounds() const;

#if WITH_EDITOR
	bool IsDoorValid(int DoorIndex) const;

#if USE_LEGACY_DATA_VALIDATION
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#else
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	FRoomDataEditorEvent OnPropertiesChanged;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
