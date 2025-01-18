/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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

#include "ReplicableObject.h"
#include "ProceduralDungeonTypes.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "RoomConnection.generated.h"

class URoom;
class ADoor;

// A DungeonGraph subobject that represents a connection between two rooms.
UCLASS()
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
	int32 GetID() const;
	const TWeakObjectPtr<URoom> GetRoomA() const;
	const TWeakObjectPtr<URoom> GetRoomB() const;
	int32 GetRoomADoorId() const;
	int32 GetRoomBDoorId() const;
	TWeakObjectPtr<URoom> GetOtherRoom(const URoom* FromRoom) const;
	int32 GetOtherDoorId(const URoom* FromRoom) const;
	bool IsDoorInstanced() const;
	ADoor* GetDoorInstance() const;
	void SetDoorClass(TSubclassOf<ADoor> DoorClass, bool bFlipped);
	ADoor* InstantiateDoor(UWorld* World, AActor* Owner = nullptr, bool bUseOwnerTransform = false);

	// Convenient functions to return a default value if the connection is nullptr.
	static URoom* GetOtherRoom(const URoomConnection* Conn, const URoom* FromRoom);
	static int32 GetOtherDoorId(const URoomConnection* Conn, const URoom* FromRoom);
	static ADoor* GetDoorInstance(const URoomConnection* Conn);
	static class UDoorType* GetDoorType(const URoomConnection* Conn);

	static URoomConnection* CreateConnection(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB, UObject* Outer, int32 IdInOuter);

private:
	UFUNCTION()
	void OnRep_ID();

	UFUNCTION()
	void OnRep_RoomA();

	UFUNCTION()
	void OnRep_RoomB();

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
	TSubclassOf<ADoor> DoorClass {nullptr};

	UPROPERTY(SaveGame)
	bool bFlipped {false};

	UPROPERTY(Replicated, Transient)
	TWeakObjectPtr<ADoor> DoorInstance {nullptr};

private:
	// Store temporary data used only during saving/loading the game
	struct FSaveData
	{
		int64 RoomAID {-1};
		int64 RoomBID {-1};
		TArray<uint8> DoorSavedData;
	};

	TUniquePtr<FSaveData> SaveData {nullptr};
};
