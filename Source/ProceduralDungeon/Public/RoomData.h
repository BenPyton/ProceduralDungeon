// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralDungeonTypes.h"
#include "RoomData.generated.h"

/**
 * 
 */
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

	UPROPERTY(EditAnywhere, Category = "Doors")
	TArray<FDoorDef> Doors;

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector Size;

public:
	URoomData();

	int GetNbDoor() const { return Doors.Num(); }
};
