// Copyright Benoit Pelletier 2019 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/DoorInterface.h"
#include "ProceduralDungeonTypes.h"
#include "DoorComponent.generated.h"

class URoom;
class UDoorType;
class UDoorComponent;
class URoomConnection;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDoorComponentLockedDelegate, UDoorComponent*, Component, bool, IsLocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDoorComponentOpenedDelegate, UDoorComponent*, Component, bool, IsOpened);

// Component that manages open/close of a door, as well as a locking state.
// Multiplayer and GameSave ready
UCLASS(BlueprintType, Blueprintable, ClassGroup = "ProceduralDungeon", meta = (BlueprintSpawnableComponent))
class PROCEDURALDUNGEON_API UDoorComponent : public UActorComponent, public IDoorInterface
{
	GENERATED_BODY()

public:
	UDoorComponent();

public:
	//~ Begin AActor interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End AActor interface

	//~ Begin IDoorInterface interface
	virtual const UDoorType* GetDoorType_Implementation() const override { return Type; }
	virtual void SetRoomConnection_Implementation(URoomConnection* RoomConnection) override;
	//~ End IDoorInterface interface

public:

	UFUNCTION(BlueprintPure, Category = "Door", meta = (CompactNodeTitle = "Is Locked"))
	FORCEINLINE bool IsLocked() const { return bLocked; }

	UFUNCTION(BlueprintPure, Category = "Door", meta = (CompactNodeTitle = "Is Open"))
	FORCEINLINE bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Door")
	void Open(bool bOpen);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Door")
	void Lock(bool bLock);

	bool ShouldBeOpen() const;
	bool ShouldBeLocked() const;

	URoom* GetRoomA() const;
	URoom* GetRoomB() const;
	URoomConnection* GetRoomConnection() const { return RoomConnection; }

	bool IsAlwaysVisible() const { return bAlwaysVisible; }
	bool IsAlwaysUnlocked() const { return bAlwaysUnlocked; }
	void SetAlwaysVisible(bool bInAlwaysVisible) { bAlwaysVisible = bInAlwaysVisible; }
	void SetAlwaysUnlocked(bool bInAlwaysUnlocked) { bAlwaysUnlocked = bInAlwaysUnlocked; }

	void SetDoorType(UDoorType* DoorType) { Type = DoorType; }

public:
	UPROPERTY(BlueprintAssignable, Category = "Door")
	FDoorComponentOpenedDelegate OnDoorOpened;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FDoorComponentLockedDelegate OnDoorLocked;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Locked"))
	void OnDoorLock_BP(bool bIsLocked);
	virtual void OnDoorLock(bool bIsLocked) {}

	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Open"))
	void OnDoorOpen_BP(bool bIsOpened);
	virtual void OnDoorOpen(bool bIsOpened) {}

	bool OwnerHasAuthority() const;

	UFUNCTION()
	void OnRep_RoomConnection();

protected:
	bool bLocked {false};
	bool bIsOpen {false};

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RoomConnection, Category = "Door")
	URoomConnection* RoomConnection {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Door", meta = (DisplayName = "Always Visible"))
	bool bAlwaysVisible {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Door", meta = (DisplayName = "Always Unlocked"))
	bool bAlwaysUnlocked {false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", meta = (DisplayName = "Door Type"))
	UDoorType* Type {nullptr};
};
