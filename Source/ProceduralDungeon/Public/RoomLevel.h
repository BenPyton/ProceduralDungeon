// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "RoomLevel.generated.h"

class URoom;

/**
 * 
 */
UCLASS()
class PROCEDURALDUNGEON_API ARoomLevel : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Data", meta = ( BlueprintBaseOnly /* Don't work... */ ) )
	TSubclassOf<class URoomData> Data;

public:
	static uint32 Count;

	UPROPERTY()
	URoom* Room = nullptr;
	bool PlayerInside = false;
	bool IsHidden = false;
	bool IsInit = false;
	bool PendingInit = false;
	bool IsLocked = false;

	UPROPERTY(EditAnywhere, Category = "Room Level")
	bool AlwaysVisible = false;

public:
	ARoomLevel(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	void Init(URoom* _room);
	uint32 GetId() { return Id; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Room Level")
	uint32 Id;
	UPROPERTY()
	TArray<AActor*> ActorsInLevel;
	FTransform Transform;
	FVector Center;
	FVector HalfExtents;

	bool IsPlayerInside();
	void Display();
};
