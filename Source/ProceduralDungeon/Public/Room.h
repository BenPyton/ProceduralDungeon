/*
 * MIT License
 *
 * Copyright (c) 2019-2022 Benoit Pelletier
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
#include "ProceduralLevelStreaming.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonTypes.h"
#include "Math/GenericOctree.h" // for FBoxCenterAndExtent (required for UE5.0)
#include "Room.generated.h"

class ADungeonGenerator;
class ARoomLevel;
class URoomData;
class ADoor;
class URoomCustomData;

USTRUCT()
struct FRoomConnection
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<URoom> OtherRoom {nullptr};
	int OtherDoorIndex {-1};
	ADoor* DoorInstance {nullptr};
};

UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API URoom : public UObject
{
	GENERATED_BODY()

public:
	// TODO: Make them private
	UPROPERTY()
	UProceduralLevelStreaming* Instance {nullptr};
	UPROPERTY()
	FIntVector Position {0};
	EDoorDirection Direction {EDoorDirection::NbDirection};

	URoom();

	const URoomData* GetRoomData() const { return RoomData; }
	const ADungeonGenerator* Generator() const { return GeneratorOwner; }
	void SetPlayerInside(bool PlayerInside);
	void SetVisible(bool Visible);

	UFUNCTION(BlueprintPure, Category = "Room")
	FORCEINLINE bool IsPlayerInside() const { return bPlayerInside; }

	UFUNCTION(BlueprintPure, Category = "Room", meta = (CompactNodeTitle = "Is Visible"))
	FORCEINLINE bool IsVisible() const { return bIsVisible; }

	UFUNCTION(BlueprintPure, Category = "Room", meta = (CompactNodeTitle = "Is Locked"))
	FORCEINLINE bool IsLocked() const { return bIsLocked; }

	UFUNCTION(BlueprintCallable, Category = "Room")
	void Lock(bool lock) { bIsLocked = lock; }

	FORCEINLINE uint64 GetRoomID() const { return Id; }

	bool CreateCustomData(const TSubclassOf<URoomCustomData>& DataType);

	UFUNCTION(BlueprintCallable, Category = "Room", meta = (ExpandBoolAsExecs = "ReturnValue", DeterminesOutputType = "DataType", DynamicOutputParam = "Data"))
	bool GetCustomData(TSubclassOf<URoomCustomData> DataType, URoomCustomData*& Data);

private:
	UPROPERTY()
	URoomData* RoomData {nullptr};

	UPROPERTY()
	TMap<UClass*, URoomCustomData*> CustomData;

	UPROPERTY()
	TArray<FRoomConnection> Connections;

	ADungeonGenerator* GeneratorOwner {nullptr};
	int64 Id {-1};
	bool bPlayerInside {false};
	bool bIsVisible {true};
	bool bIsLocked {false};

	UFUNCTION() // needed macro for binding to delegate
	void OnInstanceLoaded();

public:
	void Init(URoomData* RoomData, ADungeonGenerator* Generator, int32 RoomId);

	bool IsConnected(int Index) const;
	void SetConnection(int Index, URoom* Room, int OtherDoorIndex);
	TWeakObjectPtr<URoom> GetConnection(int Index) const;
	int GetFirstEmptyConnection() const;

	void Instantiate(UWorld* World);
	void Destroy(UWorld* World);
	ARoomLevel* GetLevelScript() const;
	bool IsInstanceLoaded() const;
	bool IsInstanceUnloaded() const;
	bool IsInstanceInitialized() const;

	EDoorDirection GetDoorWorldOrientation(int DoorIndex);
	FIntVector GetDoorWorldPosition(int DoorIndex);
	int GetConnectionCount() const { return Connections.Num(); }
	int GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsDoorInstanced(int DoorIndex);
	void SetDoorInstance(int DoorIndex, ADoor* Door);
	int GetOtherDoorIndex(int DoorIndex);
	void TryConnectToExistingDoors(TArray<URoom*>& RoomList);

	FIntVector WorldToRoom(const FIntVector& WorldPos) const;
	FIntVector RoomToWorld(const FIntVector& RoomPos) const;
	EDoorDirection WorldToRoom(const EDoorDirection& WorldRot) const;
	EDoorDirection RoomToWorld(const EDoorDirection& RoomRot) const;
	FBoxMinAndMax WorldToRoom(const FBoxMinAndMax& WorldBox) const;
	FBoxMinAndMax RoomToWorld(const FBoxMinAndMax& RoomBox) const;
	void SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot);
	void SetPositionFromDoor(int DoorIndex, FIntVector WorldPos);
	void SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot);
	bool IsOccupied(FIntVector Cell);

	FTransform GetTransform() const;
	FBoxCenterAndExtent GetBounds() const;
	FBoxCenterAndExtent GetLocalBounds() const;
	FBoxMinAndMax GetIntBounds() const;

	// AABB Overlapping
	static bool Overlap(const URoom& A, const URoom& B);
	static bool Overlap(const URoom& Room, const TArray<URoom*>& RoomList);

	static FVector GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, bool includeOffset = true);

	static void Connect(URoom& RoomA, int DoorA, URoom& RoomB, int DoorB);
	static URoom* GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList);

	// Plugin Settings
	static FVector Unit();
	static FVector DoorSize();
	static float DoorOffset();
	static bool OcclusionCulling();
	static bool UseLegacyOcclusion();
	static uint32 OcclusionDistance();
	static bool OccludeDynamicActors();
	static bool DrawDebug();
	static bool CanLoop();
};
