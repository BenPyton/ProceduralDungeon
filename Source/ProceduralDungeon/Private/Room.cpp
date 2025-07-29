// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
#include "Interfaces/RoomContainer.h"
#include "Interfaces/GeneratorProvider.h"
#include "Interfaces/RoomActorGuid.h"
#include "Utils/DungeonSaveUtils.h"
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

	if (RoomData.IsValid())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(URoom, Connections, this);
		Connections.SetNum(RoomData->GetNbDoor());
	}
	else
	{
		DungeonLog_Error("No RoomData provided.");
	}

	CreateAllCustomData();
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
		if (RoomData.IsNull())
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
			InstanceName.Appendf(TEXT("_%s_%d"), *GeneratorOwner->GetGuid().ToString(), GeneratorOwner->GetSeed());
		}
		InstanceName.Appendf(TEXT("_%d"), Id);

		FVector FinalLocation = rotation.RotateVector(RoomData->GetRoomUnit() * FVector(Position)) + offset;
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

	// @TODO: Would be great to deserialize saved actors here.
	// However, the actors at this time are not the final actors spawned in the room level.
	// So they lose their deserialized values when the real spawn occurs at a later time.
	//if (SaveData.IsValid())
	//{
	//	SerializeLevelActors(*SaveData, true);
	//}

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
	DungeonLog_Debug("[%s] Room '%s' setting IsLocked: %s", *GetAuthorityName(), *GetNameSafe(this), bIsLocked ? TEXT("True") : TEXT("False"));
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
	DungeonLog_Debug("[%s] Room '%s' IsLocked Replicated: %s", *GetAuthorityName(), *GetNameSafe(this), bIsLocked ? TEXT("True") : TEXT("False"));
}

void URoom::OnRep_Id()
{
	DungeonLog_Debug("[%s] Room '%s' Id Replicated: %d", *GetAuthorityName(), *GetNameSafe(this), Id);
}

void URoom::OnRep_RoomData()
{
	DungeonLog_Debug("[%s] Room '%s' RoomData Replicated: %s", *GetAuthorityName(), *GetNameSafe(this), *GetNameSafe(RoomData.Get()));
	// This is a hotfix to prevent the RoomData being garbage collected on clients.
	// Do not use HardRoomData anywhere else.
	HardRoomData = TStrongObjectPtr<URoomData>(RoomData.Get());
}

void URoom::OnRep_Connections()
{
	DungeonLog_Debug("[%s] Room '%s' Connections Replicated", *GetAuthorityName(), *GetNameSafe(this));
	for (const auto& Connection : Connections)
	{
		if (Connection == nullptr)
			continue;
		// This may show 'None' for all rooms depending on the order of those weakptr resolutions.
		DungeonLog_Debug("- %s (%d) <-> %s (%d)", *GetNameSafe(Connection->GetRoomA().Get()), Connection->GetRoomADoorId(), *GetNameSafe(Connection->GetRoomB().Get()), Connection->GetRoomBDoorId());
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
	check(RoomData.IsValid());
	return DoorIndex >= 0 && DoorIndex < RoomData->Doors.Num();
}

int URoom::GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot) const
{
	if (EDoorDirection::NbDirection == WorldRot)
		return -2;

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

int URoom::GetOtherDoorIndex(int32 DoorIndex) const
{
	check(IsDoorIndexValid(DoorIndex));
	return URoomConnection::GetOtherDoorId(Connections[DoorIndex].Get(), this);
}

FDoorDef URoom::GetDoorDef(int32 DoorIndex) const
{
	check(RoomData.IsValid());
	return RoomToWorld(RoomData->GetDoorDef(DoorIndex));
}

FDoorDef URoom::GetDoorDefAt(FIntVector WorldPos, EDoorDirection WorldRot) const
{
	check(RoomData.IsValid());
	int32 DoorIndex = GetDoorIndexAt(WorldPos, WorldRot);
	return (DoorIndex >= 0) ? GetDoorDef(DoorIndex) : FDoorDef::Invalid;
}

FIntVector URoom::WorldToRoom(const FIntVector& WorldPos) const
{
	return InverseTransform(WorldPos, Position, Direction);
}

FIntVector URoom::RoomToWorld(const FIntVector& RoomPos) const
{
	return Transform(RoomPos, Position, Direction);
}

EDoorDirection URoom::WorldToRoom(const EDoorDirection& WorldRot) const
{
	return InverseTransform(WorldRot, Direction);
}

EDoorDirection URoom::RoomToWorld(const EDoorDirection& RoomRot) const
{
	return Transform(RoomRot, Direction);
}

FBoxMinAndMax URoom::WorldToRoom(const FBoxMinAndMax& WorldBox) const
{
	return Rotate(WorldBox - Position, -Direction);
}

FBoxMinAndMax URoom::RoomToWorld(const FBoxMinAndMax& RoomBox) const
{
	return Rotate(RoomBox, Direction) + Position;
}

FDoorDef URoom::WorldToRoom(const FDoorDef& WorldDoor) const
{
	return FDoorDef::InverseTransform(WorldDoor, Position, Direction);
}

FDoorDef URoom::RoomToWorld(const FDoorDef& RoomDoor) const
{
	return FDoorDef::Transform(RoomDoor, Position, Direction);
}

FVoxelBounds URoom::WorldToRoom(const FVoxelBounds& WorldBounds) const
{
	return Rotate(WorldBounds - Position, -Direction);
}

FVoxelBounds URoom::RoomToWorld(const FVoxelBounds& RoomBounds) const
{
	return Rotate(RoomBounds, Direction) + Position;
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
	check(RoomData.IsValid());
	return RoomData->GetBounds(GetTransform());
}

FBoxCenterAndExtent URoom::GetLocalBounds() const
{
	check(RoomData.IsValid());
	return RoomData->GetBounds();
}

FBoxMinAndMax URoom::GetIntBounds() const
{
	check(RoomData.IsValid());
	return RoomToWorld(RoomData->GetIntBounds());
}

FVoxelBounds URoom::GetVoxelBounds() const
{
	check(RoomData.IsValid());
	return RoomToWorld(RoomData->GetVoxelBounds());
}

FTransform URoom::GetTransform() const
{
	checkf(RoomData.IsValid(), TEXT("Invalid RoomData in URoom class!"));
	FTransform Transform;
	Transform.SetLocation(FVector(Position) * RoomData->GetRoomUnit());
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

bool URoom::CreateAllCustomData()
{
	check(RoomData.IsValid());
	bool bSucceeded = true;
	for (auto Datum : RoomData->CustomData)
	{
		bSucceeded &= CreateCustomData(Datum);
	}
	return bSucceeded;
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
	if (!IsValid(Datum))
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
		if (URoomConnection::GetOtherRoom(Connection.Get(), this) != nullptr)
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

bool URoom::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
{
	check(!SaveData.IsValid());
	SaveData = MakeUnique<FSaveData>();

	if (!bIsLoading)
	{
		SaveData->ConnectionIds.SetNum(Connections.Num());
		for (int i = 0; i < Connections.Num(); ++i)
		{
			SaveData->ConnectionIds[i] = (Connections[i].IsValid()) ? Connections[i]->GetID() : -1;
		}

		// Serializing the room actors *only* during the save here.
		// They will be deserialized at a later time when loading a dungeon, once the room instance is spawned.
		SerializeLevelActors(*SaveData, bIsLoading);
	}

	int32 NumCustomData = CustomData.Num();
	FStructuredArchiveArray CustomDataRecords = Record.EnterArray(AR_FIELD_NAME("CustomData"), NumCustomData);
	CustomData.SetNum(NumCustomData);

	for (int32 i = 0; i < NumCustomData; i++)
	{
		FStructuredArchive::FRecord CustomDataRecord = CustomDataRecords.EnterElement().EnterRecord();
		SerializeUClass(CustomDataRecord.EnterField(AR_FIELD_NAME("Class")), CustomData[i].DataClass);

		if (bIsLoading)
		{
			CustomData[i].Data = NewObject<URoomCustomData>(GetOuter(), CustomData[i].DataClass);
		}

		FStructuredArchive::FRecord PropertiesRecord = CustomDataRecord.EnterRecord(AR_FIELD_NAME("Data"));
		SerializeUObject(PropertiesRecord, CustomData[i].Data, bIsLoading);
	}

	Record.EnterField(AR_FIELD_NAME("ConnectionIds")) << SaveData->ConnectionIds;
	Record.EnterField(AR_FIELD_NAME("LevelActor")) << SaveData->LevelActor;
	Record.EnterField(AR_FIELD_NAME("Actors")) << SaveData->Actors;

	if (!bIsLoading)
	{
		// When saving, no need to keep the data anymore.
		SaveData.Reset();
	}

	return false;
}

bool URoom::FixupReferences(UObject* Context)
{
	const IGeneratorProvider* GeneratorProvider = Cast<IGeneratorProvider>(Context);
	if (!GeneratorProvider)
	{
		DungeonLog_Error("Failed to fixup references: Context '%s' does not implements interface IGeneratorProvider.", *GetNameSafe(Context));
		return false;
	}

	GeneratorOwner = GeneratorProvider->GetGenerator();
	if (!GeneratorOwner.IsValid())
	{
		DungeonLog_WarningSilent("Failed to fixup references: Generator is invalid.", *GetNameSafe(Context));
		return false;
	}

	const IRoomContainer* Container = Cast<IRoomContainer>(Context);
	if (!Container)
	{
		DungeonLog_Error("Failed to fixup references: Context '%s' does not implements interface IRoomContainer.", *GetNameSafe(Context));
		return false;
	}

	if (!SaveData.IsValid())
	{
		DungeonLog_Error("Failed to fixup references: SaveData is null.");
		return false;
	}

	Connections.SetNum(SaveData->ConnectionIds.Num());
	for (int i = 0; i < SaveData->ConnectionIds.Num(); ++i)
	{
		int32 ConnectionIndex = SaveData->ConnectionIds[i];
		if (ConnectionIndex < 0)
			continue;

		URoomConnection* Connection = Container->GetConnectionByIndex(ConnectionIndex);
		if (!IsValid(Connection))
		{
			DungeonLog_Error("Failed to fixup references: Connection with index %d not found.", ConnectionIndex);
			continue;
		}

		Connections[i] = Connection;
		DungeonLog_Debug("Fixed up connection: %s (id: %d)", *GetNameSafe(Connection), ConnectionIndex);
	}

	return true;
}

void URoom::PreSaveDungeon_Implementation()
{
	DispatchCallbackToSavedLevelActors(&IDungeonSaveInterface::DispatchPreSaveEvent);
}

void URoom::PostLoadDungeon_Implementation()
{
	check(SaveData);

	// @TODO: Find a way to do it in the `OnInstanceLoaded` function.
	SerializeLevelActors(*SaveData, true);

	// The loading is finished, we can safely reset the saved data.
	SaveData.Reset();

	DispatchCallbackToSavedLevelActors(&IDungeonSaveInterface::DispatchPostLoadEvent);
}

bool URoom::SerializeLevelActors(FSaveData& Data, bool bIsLoading)
{
	if (!IsValid(Instance))
	{
		DungeonLog_Error("Can't save/load room instance's actors: Instance is invalid.");
		return false;
	}

	ULevel* Level = Instance->GetLoadedLevel();
	if (!IsValid(Level))
	{
		DungeonLog_Error("Can't save/load room instance's actors: Level is invalid.");
		return false;
	}

	ARoomLevel* LevelScript = GetLevelScript();
	checkf(IsValid(LevelScript), TEXT("RoomInstance must be loaded and the LevelScript valid when calling SerializeLevelScript"));
	SerializeUObject(Data.LevelActor, LevelScript, bIsLoading);

	for (auto& Actor : Level->Actors)
	{
		if (!IsValid(Actor))
		{
			DungeonLog_WarningSilent("An actor is invalid in the loaded room '%s'.", *GetNameSafe(Level));
			continue;
		}

		if (Actor == LevelScript)
			continue;

		UObject* GuidImplementer = IRoomActorGuid::GetImplementer(Actor);
		if (!IsValid(GuidImplementer))
			continue;

		if (!IRoomActorGuid::Execute_ShouldSaveActor(GuidImplementer))
			continue;

		FGuid ActorGuid = IRoomActorGuid::Execute_GetGuid(GuidImplementer);
		if (!ActorGuid.IsValid())
		{
			DungeonLog_Error("Actor Id is invalid for actor '%s'. Make sure you've implemented properly your 'RoomActorGuid::GetGuid'!", *GetNameSafe(Actor));
			continue;
		}

		DungeonLog_Debug("Serializing Actor %s (%s)", *GetFullNameSafe(Actor), *ActorGuid.ToString());

		if (!bIsLoading)
		{
			TArray<uint8>& ActorData = Data.Actors.Add(ActorGuid);
			SerializeUObject(ActorData, Actor, false);
		}
		else if (TArray<uint8>* ActorData = Data.Actors.Find(ActorGuid))
		{
			SerializeUObject(*ActorData, Actor, true);
		}
	}

	return true;
}

void URoom::DispatchCallbackToSavedLevelActors(TFunction<void(AActor*)> Callback) const
{
	DungeonLog_Debug("Room dispatch callback to actors.");
	if (!IsValid(Instance))
		return;

	ULevel* Level = Instance->GetLoadedLevel();
	if (!IsValid(Level))
		return;

	for (auto& Actor : Level->Actors)
	{
		if (!Actor->Implements<UDungeonSaveInterface>())
			continue;

		DungeonLog_Debug("- Dispatch to actor: %s.", *GetNameSafe(Actor));
		Callback(Actor);
	}
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
