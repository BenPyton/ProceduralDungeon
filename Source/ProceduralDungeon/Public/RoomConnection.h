// Copyright Benoit Pelletier 2025 - 2026 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "ReplicableObject.h"
#include "ProceduralDungeonTypes.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "RoomConnection.generated.h"

class URoom;

// A DungeonGraph subobject that represents a connection between two rooms.
UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API URoomConnection : public UReplicableObject, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override;
	virtual bool FixupReferences(UObject* Context) override;
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PreSaveDungeon_Implementation() override;
	virtual void PostLoadDungeon_Implementation() override;
	//~ End IDungeonSaveInterface Interface

public:
	UFUNCTION(BlueprintPure, Category = "Room Connection")
	int32 GetID() const;

	const TWeakObjectPtr<URoom> GetRoomA() const;
	const TWeakObjectPtr<URoom> GetRoomB() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection", meta = (DisplayName = "Get Door A Index"))
	int32 GetRoomADoorId() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection", meta = (DisplayName = "Get Door B Index"))
	int32 GetRoomBDoorId() const;

	TWeakObjectPtr<URoom> GetOtherRoom(const URoom* FromRoom) const;
	int32 GetOtherDoorId(const URoom* FromRoom) const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	bool IsDoorInstanced() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	AActor* GetDoorInstance() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	FVector GetDoorLocation(bool bIgnoreGeneratorTransform) const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	FRotator GetDoorRotation(bool bIgnoreGeneratorTransform) const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	bool IsDoorOpen() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection")
	bool IsDoorLocked() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Room Connection")
	void SetDoorOpen(bool bOpen);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Room Connection")
	void SetDoorLocked(bool bLocked);

	void SetDoorClass(TSubclassOf<AActor> DoorClass, bool bFlipped);
	AActor* InstantiateDoor(UWorld* World, AActor* Owner = nullptr, bool bUseOwnerTransform = false);
	void DestroyDoor();

	// Convenient functions to return a default value if the connection is nullptr.
	static URoom* GetOtherRoom(const URoomConnection* Conn, const URoom* FromRoom);
	static int32 GetOtherDoorId(const URoomConnection* Conn, const URoom* FromRoom);
	static AActor* GetDoorInstance(const URoomConnection* Conn);
	static class UDoorType* GetDoorType(const URoomConnection* Conn);
	static void GetBothDoorTypes(const URoomConnection* Conn, UDoorType*& DoorTypeA, UDoorType*& DoorTypeB);

	void Init(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB, int32 Id);
	void Reset();

protected:
	UFUNCTION(BlueprintPure, Category = "Room Connection", meta = (DisplayName = "Get Room A", CompactNodeTitle = "Room A"))
	const URoom* GetRoomA_BP() const;

	UFUNCTION(BlueprintPure, Category = "Room Connection", meta = (DisplayName = "Get Room B", CompactNodeTitle = "Room B"))
	const URoom* GetRoomB_BP() const;

private:
	UFUNCTION()
	void OnRep_ID();

	UFUNCTION()
	void OnRep_RoomA();

	UFUNCTION()
	void OnRep_RoomB();

	UFUNCTION()
	void OnRep_DoorState();

private:
	UPROPERTY(ReplicatedUsing = OnRep_ID, SaveGame)
	int32 ID {-1};

	UPROPERTY(ReplicatedUsing = OnRep_RoomA)
	TWeakObjectPtr<URoom> RoomA {nullptr};

	UPROPERTY(Replicated, SaveGame)
	int32 RoomADoorId {-1};

	UPROPERTY(ReplicatedUsing = OnRep_RoomB)
	TWeakObjectPtr<URoom> RoomB {nullptr};

	UPROPERTY(Replicated, SaveGame)
	int32 RoomBDoorId {-1};

	UPROPERTY()
	TSubclassOf<AActor> DoorClass {nullptr};

	UPROPERTY(SaveGame)
	bool bFlipped {false};

	UPROPERTY(Replicated, Transient)
	TWeakObjectPtr<AActor> DoorInstance {nullptr};

	UPROPERTY(ReplicatedUsing = OnRep_DoorState, SaveGame)
	FDoorState DoorState;

private:
	// Store temporary data used only during saving/loading the game
	struct FSaveData
	{
		int32 Version {-1};
		int64 RoomAID {-1};
		int64 RoomBID {-1};
		TArray<uint8> DoorSavedData;
	};

	TUniquePtr<FSaveData> SaveData {nullptr};
};
