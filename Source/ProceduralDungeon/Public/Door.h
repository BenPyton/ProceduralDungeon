// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Door.generated.h"

class URoom;

UCLASS()
class PROCEDURALDUNGEON_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

public:
	UFUNCTION()
	void OpenDoor();
	UFUNCTION()
	void CloseDoor();

protected:
	UFUNCTION()
	virtual void OnDoorLock() {}
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Locked"))
	void OnDoorLock_BP();

	UFUNCTION()
	virtual void OnDoorUnlock() {}
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Unlocked"))
	void OnDoorUnlock_BP();

	UFUNCTION()
	virtual void OnDoorOpen() {}
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Open"))
	void OnDoorOpen_BP();

	UFUNCTION()
	virtual void OnDoorClose() {}
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Close"))
	void OnDoorClose_BP();

protected:
	bool locked = false;
	bool isOpen = false;

	// The two connected rooms to this door
	UPROPERTY()
	URoom* RoomA;
	UPROPERTY()
	URoom* RoomB;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool AlwaysVisible = false;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool AlwaysUnlocked = false;

private:
	bool prevLocked = false;

public:
	void SetConnectingRooms(URoom* _roomA, URoom* _roomB);

	UFUNCTION(BlueprintCallable, meta = (DisplayName="Is Locked"))
	bool IsLocked() { return locked; }

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Open"))
	bool IsOpen() { return isOpen; }

	static void DrawDebug(UWorld* World, FIntVector DoorCell = FIntVector::ZeroValue, EDoorDirection DoorRot = EDoorDirection::NbDirection, FTransform Transform = FTransform::Identity);
};
