/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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

#include "Room.h"
#include "Door.h"
#include "Engine/World.h"
#include "Utils/ReplicationUtils.h"
#include "RoomData.h"
#include "RoomLevel.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGenerator.h"
#include "RoomCustomData.h"
#include "RoomConnection.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Misc/EngineVersionComparison.h"

void URoom::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(URoom, bIsLocked, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, CustomData, Params);

	// InitialOnly is not called on newly created subobjects after the InitialCond of actor owner has already been called!!!
	//Params.Condition = COND_InitialOnly;
	DOREPLIFETIME_WITH_PARAMS(URoom, RoomData, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, Position, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, Direction, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, Connections, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, GeneratorOwner, Params);
	DOREPLIFETIME_WITH_PARAMS(URoom, Id, Params);
}

bool URoom::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const auto& Pair : CustomData)
	{
		bWroteSomething |= Pair.Data->ReplicateSubobject(Channel, Bunch, RepFlags);
	}
	return bWroteSomething;
}

void URoom::RegisterReplicableSubobjects(bool bRegister)
{
	for (const auto& Pair : CustomData)
	{
		Pair.Data->RegisterAsReplicable(bRegister);
	}
}

void URoom::Init(URoomData* Data, ADungeonGeneratorBase* Generator, int32 RoomId)
{
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(RoomData, Data);
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(GeneratorOwner, Generator);
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(Id, RoomId);
	Instance = nullptr;
	SetPosition(FIntVector::ZeroValue);
	SetDirection(EDoorDirection::North);

	if (IsValid(RoomData))
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(URoom, Connections, this);
		Connections.SetNum(RoomData->GetNbDoor());
	}
	else
	{
		DungeonLog_Error("No RoomData provided.");
	}
}

bool URoom::IsConnected(int32 DoorIndex) const
{
	return GetConnectedRoom(DoorIndex) != nullptr;
}

void URoom::SetConnection(int32 DoorIndex, URoomConnection* Conn)
{
	check(Connections.IsValidIndex(DoorIndex));
	check(Connections[DoorIndex] == nullptr);
	check(Conn != nullptr && (Conn->GetRoomA() == this || Conn->GetRoomB() == this));
	Connections[DoorIndex] = Conn;
	MARK_PROPERTY_DIRTY_FROM_NAME(URoom, Connections, this);
}

TWeakObjectPtr<URoom> URoom::GetConnectedRoom(int32 DoorIndex) const
{
	check(Connections.IsValidIndex(DoorIndex));
	return URoomConnection::GetOtherRoom(Connections[DoorIndex].Get(), this);
}

int32 URoom::GetFirstEmptyConnection() const
{
	for (int i = 0; i < Connections.Num(); ++i)
	{
		if (Connections[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

void URoom::GetAllEmptyConnections(TArray<int32>& EmptyConnections) const
{
	EmptyConnections.Empty();
	for (int i = 0; i < Connections.Num(); ++i)
	{
		if (Connections[i] == nullptr)
		{
			EmptyConnections.Add(i);
		}
	}
}

void URoom::Instantiate(UWorld* World)
{
	if (Instance == nullptr)
	{
		if (!IsValid(RoomData))
		{
			DungeonLog_Error("Failed to instantiate the room: it has no RoomData.");
			return;
		}

		const TSoftObjectPtr<UWorld>& Level = RoomData->Level;
		if (Level.IsNull())
		{
			DungeonLog_Error("Failed to instantiate the room: Level asset is invalid in room data.");
			return;
		}

		FVector offset(0, 0, 0);
		FQuat rotation = FQuat::Identity;
		TStringBuilder<256> InstanceName;

		InstanceName.Append(Level.GetAssetName());
		if (GeneratorOwner.IsValid())
		{
			offset = GeneratorOwner->GetDungeonOffset();
			rotation = GeneratorOwner->GetDungeonRotation();
			InstanceName.Appendf(TEXT("_%d_%d"), GeneratorOwner->GetUniqueId(), GeneratorOwner->GetGeneration());
		}
		InstanceName.Appendf(TEXT("_%d"), Id);

		FVector FinalLocation = rotation.RotateVector(Dungeon::RoomUnit() * FVector(Position)) + offset;
		FQuat FinalRotation = rotation * ToQuaternion(Direction);
		Instance = LoadInstance(World, Level, InstanceName.ToString(), FinalLocation, FinalRotation.Rotator());

		if (!IsValid(Instance))
		{
			DungeonLog_Error("Error when instantiating the room: unknown error.");
			return;
		}

		DungeonLog_Info("[%s][R:%s][I:%s] Load room Instance: %s", *GetAuthorityName(), *GetName(), *GetNameSafe(Instance), *Instance->GetWorldAssetPackageName());
		Instance->OnLevelLoaded.AddDynamic(this, &URoom::OnInstanceLoaded);
	}
	else
	{
		DungeonLog_Error("Failed to instantiate the room: it is already instanciated.");
	}
}

void URoom::Destroy()
{
	if (IsValid(Instance))
	{
		DungeonLog_InfoSilent("[%s][R:%s][I:%s] Unload room Instance: %s", *GetAuthorityName(), *GetName(), *GetNameSafe(Instance), *Instance->GetWorldAssetPackageName());
		UnloadInstance(Instance);
	}
	else
	{
		DungeonLog_InfoSilent("[%s][R:%s] No room instance to unload", *GetAuthorityName(), *GetName());
	}
}

void URoom::OnInstanceLoaded()
{
	check(IsValid(Instance));
	Instance->OnLevelLoaded.RemoveDynamic(this, &URoom::OnInstanceLoaded);

	ARoomLevel* Script = GetLevelScript();
	if (!IsValid(Script))
	{
		DungeonLog_Error("Error when instantiating the room: the level blueprint does not derive from RoomLevel.");
		return;
	}

	Script->Init(this);

	DungeonLog_InfoSilent("[%s][R:%s][I:%s] Room loaded: %s", *GetAuthorityName(), *GetName(), *GetNameSafe(Instance), *Instance->GetWorldAssetPackageName());
}

void URoom::ForceVisibility(bool bForce)
{
	const bool bWasVisible = IsVisible();
	bForceVisible = bForce;
	if (bWasVisible != IsVisible())
		UpdateVisibility();
}

void URoom::Lock(bool bLock)
{
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(bIsLocked, bLock);
	DungeonLog_InfoSilent("[%s] Room '%s' setting IsLocked: %s", *GetAuthorityName(), *GetNameSafe(this), bIsLocked ? TEXT("True") : TEXT("False"));
}

void URoom::SetPosition(const FIntVector& NewPosition)
{
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(Position, NewPosition);
}

void URoom::SetDirection(EDoorDirection NewDirection)
{
	SET_SUBOBJECT_REPLICATED_PROPERTY_VALUE(Direction, NewDirection);
}

void URoom::UpdateVisibility() const
{
	const bool bNewVisibility = IsVisible();

	if (Dungeon::UseLegacyOcclusion())
	{
		ARoomLevel* LevelScript = GetLevelScript();
		if (IsValid(LevelScript))
			LevelScript->SetActorsVisible(bNewVisibility);
	}
	else if (IsValid(Instance))
	{
		// TODO: make the level be visible again, I don't know why it is not visible although
		// the Visible and Loaded of StreamingLevel are correctly set to true 
		// and the Loaded of Level instance inside it is also set to true...
		// In the meantime, only the legacy version will remains.
		//Instance->SetShouldBeVisible(bNewVisibility);
		//Instance->BroadcastLevelVisibleStatus(Instance->GetWorld(), Instance->GetWorldAssetPackageFName(), bNewVisibility);
	}
}

void URoom::OnRep_IsLocked()
{
	DungeonLog_InfoSilent("[%s] Room '%s' IsLocked Replicated: %s", *GetAuthorityName(), *GetNameSafe(this), bIsLocked ? TEXT("True") : TEXT("False"));
}

void URoom::OnRep_Id()
{
	DungeonLog_InfoSilent("[%s] Room '%s' Id Replicated: %d", *GetAuthorityName(), *GetNameSafe(this), Id);
}

void URoom::OnRep_RoomData()
{
	DungeonLog_InfoSilent("[%s] Room '%s' RoomData Replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomData));
}

void URoom::OnRep_Connections()
{
	DungeonLog_InfoSilent("[%s] Room '%s' Connections Replicated", *GetAuthorityName(), *GetNameSafe(this));
	for (const auto& Connection : Connections)
	{
		if (Connection == nullptr)
			continue;
		// This may show 'None' for all rooms depending on the order of those weakptr resolutions.
		DungeonLog_InfoSilent("- %s (%d) <-> %s (%d)", *GetNameSafe(Connection->GetRoomA().Get()), Connection->GetRoomADoorId(), *GetNameSafe(Connection->GetRoomB().Get()), Connection->GetRoomBDoorId());
	}
}

ARoomLevel* URoom::GetLevelScript() const
{
	if (Instance == nullptr || !IsValid(Instance))
	{
		return nullptr;
	}
	return Cast<ARoomLevel>(Instance->GetLevelScriptActor());
}

bool URoom::IsInstanceLoaded() const
{
	if (!IsValid(Instance))
		return false;

#if UE_VERSION_OLDER_THAN(5, 2, 0)
	return Instance->GetCurrentState() > ULevelStreaming::ECurrentState::Loading;
#else
	return Instance->GetLevelStreamingState() > ELevelStreamingState::Loading;
#endif
}

bool URoom::IsInstanceUnloaded() const
{
	if (!IsValid(Instance))
		return true;

#if UE_VERSION_OLDER_THAN(5, 2, 0)
	return Instance->GetCurrentState() <= ULevelStreaming::ECurrentState::Removed;
#else
	return Instance->GetLevelStreamingState() <= ELevelStreamingState::Removed;
#endif
}

bool URoom::IsInstanceInitialized() const
{
	ARoomLevel* Script = GetLevelScript();
	return (IsValid(Script)) ? Script->IsInit() : false;
}

void URoom::CreateLevelComponents(ARoomLevel* LevelActor)
{
	for (const auto& Pair : CustomData)
	{
		if (!IsValid(Pair.Data))
			continue;
		Pair.Data->CreateLevelComponent(LevelActor);
	}
}

EDoorDirection URoom::GetDoorWorldOrientation(int DoorIndex) const
{
	check(IsDoorIndexValid(DoorIndex));
	return RoomData->Doors[DoorIndex].Direction + Direction;
}

FIntVector URoom::GetDoorWorldPosition(int DoorIndex) const
{
	check(IsDoorIndexValid(DoorIndex));
	return RoomToWorld(RoomData->Doors[DoorIndex].Position);
}

bool URoom::IsDoorIndexValid(int32 DoorIndex) const
{
	return DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num();
}

int URoom::GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot)
{
	FIntVector localPos = WorldToRoom(WorldPos);
	EDoorDirection localRot = WorldToRoom(WorldRot);

	for (int i = 0; i < RoomData->Doors.Num(); ++i)
	{
		const FDoorDef door = RoomData->Doors[i];
		if (door.Position == localPos && door.Direction == localRot)
			return i;
	}
	return -1;
}

int URoom::GetOtherDoorIndex(int32 DoorIndex)
{
	check(IsDoorIndexValid(DoorIndex));
	return URoomConnection::GetOtherDoorId(Connections[DoorIndex].Get(), this);
}

FIntVector URoom::WorldToRoom(const FIntVector& WorldPos) const
{
	return Rotate(WorldPos - Position, -Direction);
}

FIntVector URoom::RoomToWorld(const FIntVector& RoomPos) const
{
	return Rotate(RoomPos, Direction) + Position;
}

EDoorDirection URoom::WorldToRoom(const EDoorDirection& WorldRot) const
{
	return WorldRot - Direction;
}

EDoorDirection URoom::RoomToWorld(const EDoorDirection& RoomRot) const
{
	return RoomRot + Direction;
}

FBoxMinAndMax URoom::WorldToRoom(const FBoxMinAndMax& WorldBox) const
{
	return Rotate(WorldBox - Position, -Direction);
}

FBoxMinAndMax URoom::RoomToWorld(const FBoxMinAndMax& RoomBox) const
{
	return Rotate(RoomBox, Direction) + Position;
}

void URoom::SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot)
{
	check(IsDoorIndexValid(DoorIndex));
	SetDirection(WorldRot - RoomData->Doors[DoorIndex].Direction);
}

void URoom::SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)
{
	check(IsDoorIndexValid(DoorIndex));
	SetPosition(WorldPos - Rotate(RoomData->Doors[DoorIndex].Position, Direction));
}

void URoom::SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot)
{
	check(IsDoorIndexValid(DoorIndex));
	SetDirection(WorldRot - RoomData->Doors[DoorIndex].Direction);
	SetPosition(WorldPos - Rotate(RoomData->Doors[DoorIndex].Position, Direction));
}

bool URoom::IsOccupied(FIntVector Cell)
{
	FIntVector local = WorldToRoom(Cell);
	FBoxMinAndMax Bounds = RoomData->GetIntBounds();
	return local.X >= Bounds.Min.X && local.X < Bounds.Max.X
		&& local.Y >= Bounds.Min.Y && local.Y < Bounds.Max.Y
		&& local.Z >= Bounds.Min.Z && local.Z < Bounds.Max.Z;
}

FBoxCenterAndExtent URoom::GetBounds() const
{
	check(IsValid(RoomData));
	return RoomData->GetBounds(GetTransform());
}

FBoxCenterAndExtent URoom::GetLocalBounds() const
{
	check(IsValid(RoomData));
	return RoomData->GetBounds();
}

FBoxMinAndMax URoom::GetIntBounds() const
{
	check(IsValid(RoomData));
	return RoomToWorld(RoomData->GetIntBounds());
}

FTransform URoom::GetTransform() const
{
	FTransform Transform;
	Transform.SetLocation(FVector(Position) * Dungeon::RoomUnit());
	Transform.SetRotation(ToQuaternion(Direction));
	return Transform;
}

void URoom::SetVisible(bool Visible)
{
	const bool bWasVisible = IsVisible();
	bIsVisible = Visible;
	if (bWasVisible != IsVisible())
		UpdateVisibility();
}

void URoom::SetPlayerInside(bool PlayerInside)
{
	if (bPlayerInside == PlayerInside)
		return;

	bPlayerInside = PlayerInside;
}

bool URoom::CreateCustomData(const TSubclassOf<URoomCustomData>& DataType)
{
	if (DataType == nullptr)
		return false;

	// No duplicate allowed
	if (HasCustomData(DataType))
		return false;

	CustomData.Add({DataType, NewObject<URoomCustomData>(GetOuter(), DataType)});
	MARK_PROPERTY_DIRTY_FROM_NAME(URoom, CustomData, this);
	return true;
}

bool URoom::GetCustomData_BP(TSubclassOf<URoomCustomData> DataType, URoomCustomData*& Data)
{
	return GetCustomData(DataType, Data);
}

bool URoom::HasCustomData_BP(const TSubclassOf<URoomCustomData>& DataType)
{
	return HasCustomData(DataType);
}

bool URoom::GetCustomData(const TSubclassOf<URoomCustomData>& DataType, URoomCustomData*& Data) const
{
	const FCustomDataPair* Pair = GetDataPair(DataType);
	if (!Pair) // Not found
		return false;

	URoomCustomData* Datum = Pair->Data;
	if(!IsValid(Datum))
		return false;

	if (!Datum->IsA(DataType))
		return false;

	Data = Datum;
	return true;
}

bool URoom::HasCustomData(const TSubclassOf<URoomCustomData>& DataType) const
{
	return GetDataPair(DataType) != nullptr;
}

const FCustomDataPair* URoom::GetDataPair(const TSubclassOf<URoomCustomData>& DataType) const
{
	return CustomData.FindByPredicate([&DataType](const FCustomDataPair& Pair) { return Pair.DataClass == DataType; });
}

FRandomStream URoom::GetRandomStream() const
{
	if (!GeneratorOwner.IsValid())
		return FRandomStream();
	return GeneratorOwner->GetRandomStream();
}

ADoor* URoom::GetDoor(int32 DoorIndex) const
{
	if (!Connections.IsValidIndex(DoorIndex))
		return nullptr;
	return URoomConnection::GetDoorInstance(Connections[DoorIndex].Get());
}

void URoom::GetAllDoors(TArray<ADoor*>& OutDoors) const
{
	OutDoors.Reset();
	for (const auto& Connection : Connections)
	{
		ADoor* Door = URoomConnection::GetDoorInstance(Connection.Get());
		if (IsValid(Door))
			OutDoors.Add(Door);
	}
}

bool URoom::IsDoorConnected(int DoorIndex) const
{
	if (!Connections.IsValidIndex(DoorIndex))
	{
		DungeonLog_WarningSilent("Door index out of bounds.");
		return false;
	}
	return IsConnected(DoorIndex);
}

bool URoom::AreAllDoorsConnected() const
{
	for (const auto& Connection : Connections)
	{
		if (URoomConnection::GetOtherRoom(Connection.Get(), this) == nullptr)
			return false;
	}
	return true;
}

int URoom::CountConnectedDoors() const
{
	int ConnectedDoors = 0;
	for (const auto& Connection : Connections)
	{
		if (URoomConnection::GetOtherRoom(Connection.Get(), this) == nullptr)
			++ConnectedDoors;
	}
	return ConnectedDoors;
}

URoom* URoom::GetConnectedRoomAt(int DoorIndex) const
{
	if (!Connections.IsValidIndex(DoorIndex))
	{
		DungeonLog_WarningSilent("Door index out of bounds.");
		return nullptr;
	}
	return GetConnectedRoom(DoorIndex).Get();
}

void URoom::GetAllConnectedRooms(TArray<URoom*>& ConnectedRooms) const
{
	ConnectedRooms.Empty();
	for (const auto& Connection : Connections)
	{
		URoom* ConnectedRoom = URoomConnection::GetOtherRoom(Connection.Get(), this);
		if (IsValid(ConnectedRoom))
			ConnectedRooms.Add(ConnectedRoom);
	}
}

int32 URoom::GetConnectedRoomIndex(const URoom* OtherRoom) const
{
	for (int i = 0; i < Connections.Num(); ++i)
	{
		if (OtherRoom == URoomConnection::GetOtherRoom(Connections[i].Get(), this))
			return i;
	}
	return -1;
}

void URoom::GetDoorsWith(const URoom* OtherRoom, TArray<ADoor*>& Doors) const
{
	Doors.Empty();
	for (const auto& Connection : Connections)
	{
		if (OtherRoom != URoomConnection::GetOtherRoom(Connection.Get(), this))
			continue;

		ADoor* Door = URoomConnection::GetDoorInstance(Connection.Get());
		if (IsValid(Door))
			Doors.Add(Door);
	}
}

FVector URoom::GetBoundsCenter() const
{
	FVector Center = GetBounds().Center;
	if (GeneratorOwner.IsValid())
	{
		Center = GeneratorOwner->GetDungeonTransform().TransformPositionNoScale(Center);
	}
	return Center;
}

FVector URoom::GetBoundsExtent() const
{
	return GetBounds().Extent;
}

// AABB Overlapping
bool URoom::Overlap(const URoom& A, const URoom& B)
{
	FBoxMinAndMax BoxA = A.GetIntBounds();
	FBoxMinAndMax BoxB = B.GetIntBounds();
	return FBoxMinAndMax::Overlap(BoxA, BoxB);
}

bool URoom::Overlap(const URoom& Room, const TArray<URoom*>& RoomList)
{
	bool overlap = false;
	for (int i = 0; i < RoomList.Num() && !overlap; i++)
	{
		if (Overlap(Room, *RoomList[i]))
		{
			overlap = true;
		}
	}
	return overlap;
}

URoom* URoom::GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList)
{
	for (URoom* Room : RoomList)
	{
		if (IsValid(Room) && Room->IsOccupied(RoomCell))
		{
			return Room;
		}
	}
	return nullptr;
}

ULevelStreamingDynamic* URoom::LoadInstance(UObject* WorldContextObject, const TSoftObjectPtr<UWorld>& Level, const FString& InstanceNameSuffix, FVector Location, FRotator Rotation)
{
	DungeonLog_InfoSilent("[W:%s] Loading LevelStreamingDynamic", *GetNameSafe(WorldContextObject));

	if (Level.IsNull())
	{
		DungeonLog_Error("Failed to load level instance: Level is invalid.");
		return nullptr;
	}

	bool success = false;
	ULevelStreamingDynamic* Instance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(WorldContextObject, Level, Location, Rotation, success, InstanceNameSuffix);
	Instance->bDisableDistanceStreaming = true; // Make sure the level will not be streamed out by distance. We have our own streaming logic.

	if (!success)
	{
		DungeonLog_Error("Failed to load level instance: Unknown reason");
		return nullptr;
	}

	return Instance;
}

void URoom::UnloadInstance(ULevelStreamingDynamic* Instance)
{
	DungeonLog_InfoSilent("[I:%s] Unloading LevelStreamingDynamic", *GetNameSafe(Instance));
	if (nullptr == Instance)
	{
		DungeonLog_Error("Failed to unload LevelStreamingDynamic: Instance is null");
		return;
	}

	Instance->SetIsRequestingUnloadAndRemoval(true);
}
