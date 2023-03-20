/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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
#include "RoomData.generated.h"

UCLASS()
class PROCEDURALDUNGEON_API URoomData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class UProceduralLevelStreaming;

private:
	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> Level;

public:
	UPROPERTY(EditAnywhere, Category = "Door")
	bool RandomDoor;

	UPROPERTY(EditAnywhere, Category = "Doors", meta = (TitleProperty = "EdName"))
	TArray<FDoorDef> Doors;

	UPROPERTY(EditAnywhere, Category = "Room", meta = (ClampMin = 1))
	FIntVector Size;

public:
	URoomData();

	int GetNbDoor() const { return Doors.Num(); }

	FBoxCenterAndExtent GetBounds(FTransform Transform = FTransform::Identity) const;

#if WITH_EDITOR
	bool IsDoorValid(int DoorIndex) const;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	virtual void PostLoad();
};
