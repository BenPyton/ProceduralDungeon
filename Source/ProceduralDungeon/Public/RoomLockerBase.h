// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "RoomLockerBase.generated.h"

class URoomData;
class ARoomLevel;

UCLASS()
class PROCEDURALDUNGEON_API ARoomLockerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Set the room where this actor is locked or not (with self parameter) and the neighbor rooms of RoomType.
	void SetLocked(bool Locked, bool Self = true, TSubclassOf<URoomData> RoomType = nullptr);

protected:
	ARoomLevel* GetRoomLevel();
};
