/*
 * Copyright (c) 2019-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Door.generated.h"

class URoom;
class UDoorType;

// Base class for all door actors in the dungeon.
// Use this class even if you want to create a wall to place instead of a door (when the door is not connected to another room for example).
UCLASS(Blueprintable, ClassGroup = "Procedural Dungeon")
class PROCEDURALDUNGEON_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

public:
	void SetConnectingRooms(URoom* RoomA, URoom* RoomB);

	UFUNCTION(BlueprintPure, Category = "Door", meta = (CompactNodeTitle = "Is Locked"))
	FORCEINLINE bool IsLocked() const { return bLocked; }
	UFUNCTION(BlueprintPure, Category = "Door", meta = (CompactNodeTitle = "Is Open"))
	FORCEINLINE bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Door")
	void Open(bool open);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Door")
	void Lock(bool lock);

	const UDoorType* GetDoorType() const { return Type; }

	bool ShouldBeOpened() const { return bShouldBeOpen; }
	bool ShouldBeLocked() const { return bShouldBeLocked; }

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
	bool bLocked {false};
	bool bIsOpen {false};

	UPROPERTY(Replicated, SaveGame)
	bool bShouldBeLocked {false};

	UPROPERTY(Replicated, SaveGame)
	bool bShouldBeOpen {false};

	// The two connected rooms to this door
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Door")
	URoom* RoomA {nullptr};
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Door")
	URoom* RoomB {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Door", meta = (DisplayName = "Always Visible"))
	bool bAlwaysVisible {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Door", meta = (DisplayName = "Always Unlocked"))
	bool bAlwaysUnlocked {false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", meta = (DisplayName = "Door Type"))
	UDoorType* Type {nullptr};

	UPROPERTY(EditAnywhere, Category = "Door")
	USceneComponent* DefaultSceneComponent {nullptr};
};
