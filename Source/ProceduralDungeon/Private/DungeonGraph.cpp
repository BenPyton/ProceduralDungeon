// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "DungeonGraph.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonLog.h"
#include "Containers/Queue.h"
#include "DungeonGenerator.h"
#include "Room.h"
#include "RoomData.h"
#include "RoomCustomData.h"
#include "RoomConnection.h"
#include "Door.h"
#include "Engine/Level.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Utils/DungeonSaveUtils.h"
#include "ProceduralDungeonUtils.h"
#include "DungeonSettings.h"

void UDungeonGraph::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(UDungeonGraph, ReplicatedRooms, Params);
	DOREPLIFETIME_WITH_PARAMS(UDungeonGraph, RoomConnections, Params);
}

bool UDungeonGraph::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (URoom* Room : ReplicatedRooms)
	{
		check(Room);
		bWroteSomething |= Room->ReplicateSubobject(Channel, Bunch, RepFlags);
	}
	for (URoomConnection* Conn : RoomConnections)
	{
		check(Conn);
		bWroteSomething |= Conn->ReplicateSubobject(Channel, Bunch, RepFlags);
	}
	return bWroteSomething;
}

void UDungeonGraph::RegisterReplicableSubobjects(bool bRegister)
{
	for (URoom* Room : ReplicatedRooms)
	{
		Room->RegisterAsReplicable(bRegister);
	}

	for (URoomConnection* Conn : RoomConnections)
	{
		Conn->RegisterAsReplicable(bRegister);
	}
}

URoomConnection* UDungeonGraph::GetConnectionByIndex(int32 Index) const
{
	if (!RoomConnections.IsValidIndex(Index))
	{
		DungeonLog_WarningSilent("Invalid index %d for RoomConnections.", Index);
		return nullptr;
	}
	return RoomConnections[Index];
}

bool UDungeonGraph::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
{
	SavedData = MakeUnique<FSaveData>();

	if (!bIsLoading)
	{
		SavedData->Rooms = TArray<URoom*>(Rooms);
		SavedData->Connections = TArray<URoomConnection*>(RoomConnections);
	}

	SerializeUObjectArray(Record, AR_FIELD_NAME("Rooms"), SavedData->Rooms, bIsLoading, this);
	SerializeUObjectArray(Record, AR_FIELD_NAME("Connections"), SavedData->Connections, bIsLoading, this);

	return true;
}

void UDungeonGraph::PostLoadDungeon_Implementation()
{
	// Load has ended, we can safely reset the saved data.
	SavedData.Reset();
}

void UDungeonGraph::AddRoom(URoom* Room)
{
	Rooms.Add(Room);
	UpdateBounds(Room);
}

void UDungeonGraph::InitRooms()
{
	// We split the for loops to ensure custom data are created for all rooms before initializing them

	// First create empty connections for remaining unconnected doors
	TArray<int32> EmptyConnections;
	for (URoom* Room : Rooms)
	{
		check(IsValid(Room));
		Room->GetAllEmptyConnections(EmptyConnections);
		for (int32 DoorIndex : EmptyConnections)
		{
			Connect(Room, DoorIndex, nullptr, -1);
		}
	}

	// Finally we can initialize them all
	for (URoom* Room : Rooms)
	{
		// No need to check validity here
		const URoomData* Data = Room->GetRoomData();
		Data->InitializeRoom(Room, this);
	}
}

bool UDungeonGraph::TryConnectDoor(URoom* Room, int32 DoorIndex)
{
	check(IsValid(Room));

	// Check if already connected.
	if (Room->IsConnected(DoorIndex))
		return true;

	// Get the room in front of the door if any.
	EDoorDirection DoorDir = Room->GetDoorWorldOrientation(DoorIndex);
	FIntVector AdjacentCell = Room->GetDoorWorldPosition(DoorIndex) + ToIntVector(DoorDir);
	URoom* OtherRoom = GetRoomAt(AdjacentCell);
	if (!IsValid(OtherRoom))
	{
		return false;
	}

	// Get the door index of the other room if any.
	int OtherDoorIndex = OtherRoom->GetDoorIndexAt(AdjacentCell, ~DoorDir);
	if (OtherDoorIndex < 0) // -1 if no door
	{
		return false;
	}

	// Check door compatibility.
	const FDoorDef& ThisDoor = Room->GetRoomData()->Doors[DoorIndex];
	const FDoorDef& OtherDoor = OtherRoom->GetRoomData()->Doors[OtherDoorIndex];
	if (!FDoorDef::AreCompatible(ThisDoor, OtherDoor))
	{
		return false;
	}

	// Finally connect the doors.
	Connect(Room, DoorIndex, OtherRoom, OtherDoorIndex);
	return true;
}

bool UDungeonGraph::TryConnectToExistingDoors(URoom* Room)
{
	bool HasConnection = false;
	for (int i = 0; i < Room->GetRoomData()->GetNbDoor(); ++i)
	{
		HasConnection |= TryConnectDoor(Room, i);
	}
	return HasConnection;
}

void UDungeonGraph::RetrieveRoomsFromLoadedData()
{
	if (Rooms.Num() > 0)
	{
		DungeonLog_Error("Trying to retrieve loaded rooms while previous ones are not unloaded!");
		return;
	}

	if (!SavedData.IsValid())
		return;

	Rooms = TArray<URoom*>(SavedData->Rooms);
	RoomConnections = TArray<URoomConnection*>(SavedData->Connections);

	IDungeonCustomSerialization::DispatchFixupReferences(this, this);

	RebuildBounds();
}

void UDungeonGraph::Connect(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB)
{
	URoomConnection* NewConnection = URoomConnection::CreateConnection(RoomA, DoorA, RoomB, DoorB, this, RoomConnections.Num());
	RoomConnections.Add(NewConnection);
	DungeonLog_Debug("Connected %s (%d) to %s (%d)", *GetNameSafe(RoomA), DoorA, *GetNameSafe(RoomB), DoorB);
	MARK_PROPERTY_DIRTY_FROM_NAME(UDungeonGraph, RoomConnections, this);
}

void UDungeonGraph::GetAllRoomsFromData(const URoomData* Data, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [Data](const URoom* Room) { return Room->GetRoomData() == Data; });
}

void UDungeonGraph::GetAllRoomsFromDataList(const TArray<URoomData*>& Data, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [&Data](const URoom* Room) { return Data.Contains(Room->GetRoomData()); });
}

const URoom* UDungeonGraph::GetFirstRoomFromData(const URoomData* Data)
{
	return FindFirstRoomByPredicate([Data](const URoom* Room) { return Room->GetRoomData() == Data; });
}

void UDungeonGraph::GetAllRoomsWithCustomData(const TSubclassOf<URoomCustomData>& CustomData, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [&CustomData](const URoom* Room) { return Room->HasCustomData(CustomData); });
}

void UDungeonGraph::GetAllRoomsWithAllCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [&CustomData](const URoom* Room) {
		for (auto Datum : CustomData)
		{
			if (!Room->HasCustomData(Datum))
				return false;
		}
		return true;
	});
}

void UDungeonGraph::GetAllRoomsWithAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& OutRooms)
{
	GetRoomsByPredicate(OutRooms, [&CustomData](const URoom* Room) {
		for (auto Datum : CustomData)
		{
			if (Room->HasCustomData(Datum))
				return true;
		}
		return false;
	});
}

URoom* UDungeonGraph::GetRandomRoom(const TArray<URoom*>& RoomList) const
{
	if (!Generator.IsValid())
	{
		DungeonLog_Error("DungeonGraph has no Generator set.");
		return nullptr;
	}

	if (!HasRooms())
		return nullptr;

	int32 rand = Generator->GetRandomStream().FRandRange(0, RoomList.Num() - 1);
	return RoomList[rand];
}

bool UDungeonGraph::HasAlreadyRoomData(const URoomData* RoomData) const
{
	return CountRoomData(RoomData) > 0;
}

bool UDungeonGraph::HasAlreadyOneRoomDataFrom(const TArray<URoomData*>& RoomDataList) const
{
	return CountTotalRoomData(RoomDataList) > 0;
}

int UDungeonGraph::CountRoomData(const URoomData* RoomData) const
{
	return CountRoomByPredicate([RoomData](const URoom* Room) { return Room->GetRoomData() == RoomData; });
}

int UDungeonGraph::CountTotalRoomData(const TArray<URoomData*>& RoomDataList) const
{
	return CountRoomByPredicate([&RoomDataList](const URoom* Room) { return RoomDataList.Contains(Room->GetRoomData()); });
}

bool UDungeonGraph::HasAlreadyRoomType(const TSubclassOf<URoomData>& RoomType) const
{
	return CountRoomType(RoomType) > 0;
}

bool UDungeonGraph::HasAlreadyOneRoomTypeFrom(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const
{
	return CountTotalRoomType(RoomTypeList) > 0;
}

int UDungeonGraph::CountRoomType(const TSubclassOf<URoomData>& RoomType) const
{
	return CountRoomByPredicate([&RoomType](const URoom* Room) { return Room->GetRoomData()->GetClass()->IsChildOf(RoomType); });
}

int UDungeonGraph::CountTotalRoomType(const TArray<TSubclassOf<URoomData>>& RoomTypeList) const
{
	return CountRoomByPredicate([&RoomTypeList](const URoom* Room) {
		return RoomTypeList.ContainsByPredicate([Room](const TSubclassOf<URoomData> RoomType) {
			return Room->GetRoomData()->GetClass()->IsChildOf(RoomType);
		});
	});
}

bool UDungeonGraph::HasValidPath(const URoom* From, const URoom* To, bool IgnoreLockedRooms) const
{
	return FindPath(From, To, nullptr, IgnoreLockedRooms);
}

int32 UDungeonGraph::NumberOfRoomBetween(const URoom* A, const URoom* B, bool IgnoreLockedRooms) const
{
	TArray<const URoom*> Path;
	FindPath(A, B, &Path, IgnoreLockedRooms);
	return Path.Num();
}

int32 UDungeonGraph::NumberOfRoomBetween_ReadOnly(TScriptInterface<IReadOnlyRoom> A, TScriptInterface<IReadOnlyRoom> B) const
{
	// @TODO: That's not really safe, it should be better to make a FindPath using ReadOnlyRooms too.
	const URoom* RoomA = Cast<URoom>(A.GetObject());
	const URoom* RoomB = Cast<URoom>(B.GetObject());
	return NumberOfRoomBetween(RoomA, RoomB);
}

bool UDungeonGraph::GetPathBetween(const URoom* A, const URoom* B, TArray<URoom*>& ResultPath, bool IgnoreLockedRooms) const
{
	// @HACK: is it another alternative?
	TArray<const URoom*>& Temp = reinterpret_cast<TArray<const URoom*>&>(ResultPath);
	FindPath(A, B, &Temp, IgnoreLockedRooms);
	return ResultPath.Num() > 0;
}

URoom* UDungeonGraph::GetRoomAt(FIntVector RoomCell) const
{
	return URoom::GetRoomAt(RoomCell, Rooms);
}

FVector UDungeonGraph::GetDungeonBoundsCenter() const
{
	FTransform Transform = Generator.IsValid() ? Generator->GetDungeonTransform() : FTransform::Identity;
	return GetDungeonBounds(Transform).Center;
}

FVector UDungeonGraph::GetDungeonBoundsExtent() const
{
	FTransform Transform = Generator.IsValid() ? Generator->GetDungeonTransform() : FTransform::Identity;
	return GetDungeonBounds(Transform).Extent;
}

struct FRoomCandidatePredicate
{
	bool operator()(const FRoomCandidate& A, const FRoomCandidate& B) const
	{
		return A.Score > B.Score;
	}
};

bool UDungeonGraph::FilterAndSortRooms(const TArray<URoomData*>& RoomList, const FDoorDef& FromDoor, TArray<FRoomCandidate>& SortedRooms, const FScoreCallback& CustomScore) const
{
	SortedRooms.Empty();

	FDoorDef TargetDoor = FromDoor.GetOpposite();

	for (URoomData* RoomData : RoomList)
	{
		if (!IsValid(RoomData))
			continue;

		FVoxelBounds DataBounds = RoomData->GetVoxelBounds();

		// Try each possible door
		for (int i = 0; i < RoomData->GetNbDoor(); ++i)
		{
			const FDoorDef& Door = RoomData->Doors[i];

			// Filter out the door candidate if not compatible with the door
			// we want to connect from.
			if (!FDoorDef::AreCompatible(TargetDoor, Door))
				continue;

			// Compute new room placement
			const EDoorDirection RoomDirection = TargetDoor.Direction - Door.Direction;
			const FIntVector RoomLocation = TargetDoor.Position - Rotate(Door.Position, RoomDirection);

			// Filter out the rooms that does not pass the constraints
			if (!URoomData::DoesPassAllConstraints(RoomData, RoomLocation, RoomDirection))
				continue;

			FRoomCandidate Candidate;
			Candidate.Data = RoomData;
			Candidate.DoorIndex = i;

			// Check if the new bounds placed at the target door can fit
			const FVoxelBounds NewBounds = Rotate(DataBounds, RoomDirection) + RoomLocation;
			if (!NewBounds.GetCompatibilityScore(Bounds, Candidate.Score, CustomScore))
				continue;

			SortedRooms.HeapPush(Candidate, FRoomCandidatePredicate());
		}
	}

	return SortedRooms.Num() > 0;
}

bool UDungeonGraph::FilterAndSortRooms(const TArray<URoomData*>& RoomList, const FDoorDef& FromDoor, TArray<FRoomCandidate>& SortedRooms) const
{
	return FilterAndSortRooms(RoomList, FromDoor, SortedRooms, FScoreCallback());
}

FBoxCenterAndExtent UDungeonGraph::GetDungeonBounds(const FTransform& Transform) const
{
	const FVector RoomUnit = UDungeonSettings::GetRoomUnit(Generator.IsValid() ? Generator->SettingsOverrides : nullptr);
	return Dungeon::ToWorld(Bounds.GetBounds(), RoomUnit, Transform);
}

FBoxMinAndMax UDungeonGraph::GetIntBounds() const
{
	return Bounds.GetBounds();
}

URoom* UDungeonGraph::GetRoomByIndex(int64 Index) const
{
	for (URoom* Room : Rooms)
	{
		if (Room->GetRoomID() == Index)
			return Room;
	}
	return nullptr;
}

void UDungeonGraph::Clear()
{
	for (URoom* Room : Rooms)
	{
		check(IsValid(Room));
		const URoomData* Data = Room->GetRoomData();
		check(IsValid(Data));
		Data->CleanupRoom(Room, this);
	}
	Rooms.Empty();

	RoomConnections.Empty();

	RebuildBounds();
}

int UDungeonGraph::CountRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const
{
	int count = 0;
	for (const URoom* Room : Rooms)
	{
		if (Predicate(Room))
			count++;
	}
	return count;
}

void UDungeonGraph::GetRoomsByPredicate(TArray<URoom*>& OutRooms, TFunction<bool(const URoom*)> Predicate) const
{
	OutRooms.Empty();
	for (URoom* Room : Rooms)
	{
		check(IsValid(Room));
		if (Predicate(Room))
			OutRooms.Add(Room);
	}
}

const URoom* UDungeonGraph::FindFirstRoomByPredicate(TFunction<bool(const URoom*)> Predicate) const
{
	for (URoom* Room : Rooms)
	{
		check(IsValid(Room));
		if (Predicate(Room))
			return Room;
	}
	return nullptr;
}

void UDungeonGraph::TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*, uint32)> Func)
{
	TSet<URoom*> openList(InRooms);
	TSet<URoom*> closedList, currentList;
	const uint32 MaxDistance = Distance;
	while (Distance > 0 && openList.Num() > 0)
	{
		for (URoom* openRoom : openList)
			closedList.Add(openRoom);

		Swap(currentList, openList);
		openList.Empty();
		for (URoom* currentRoom : currentList)
		{
			Func(currentRoom, MaxDistance - Distance);
			for (int i = 0; i < currentRoom->GetConnectionCount(); ++i)
			{
				URoom* nextRoom = currentRoom->GetConnectedRoom(i).Get();
				if (IsValid(nextRoom) && !closedList.Contains(nextRoom))
					openList.Add(nextRoom);
			}
		}
		--Distance;
	}

	if (OutRooms != nullptr)
		Swap(*OutRooms, closedList);
}

// Do one cycle of BFS (dequeue one room from Queue, then check all its connections to add them in MarkedThis and filling ParentMap)
// Fills OutCommon  if a connection has been found in MarkedOther
// Returns true if OutCommon had been filled
bool BFS_Cycle(TQueue<const URoom*>& Queue, TSet<const URoom*>& MarkedThis, const TSet<const URoom*>& MarkedOther, TMap<const URoom*, const URoom*>& ParentMap, const URoom*& OutCommon, bool IgnoreLocked)
{
	const URoom* Current = nullptr;
	const URoom* Next = nullptr;

	Queue.Dequeue(Current);
	// for each neighbor, if not locked or marked, add it to queue and mark it
	for (int i = 0; OutCommon == nullptr && i < Current->GetConnectionCount(); ++i)
	{
		Next = Current->GetConnectedRoom(i).Get();
		if (Next && (IgnoreLocked || !Next->IsLocked()) && !MarkedThis.Contains(Next))
		{
			ParentMap.Add(Next, Current);

			// Check intersection with other side
			if (MarkedOther.Contains(Next))
			{
				OutCommon = Next;
			}
			else
			{
				Queue.Enqueue(Next);
				MarkedThis.Add(Next);
			}
		}
	}

	return OutCommon != nullptr;
}

void ReconstructPath(const URoom* Common, const TMap<const URoom*, const URoom*>& ParentsForward, const TMap<const URoom*, const URoom*>& ParentsReverse, TArray<const URoom*>& OutPath)
{
	OutPath.Empty();

	if (Common == nullptr)
		return;

	// Adds the first part of the path (From -> Common)
	const URoom* const* Current = &Common;
	while ((Current = ParentsForward.Find(*Current)) != nullptr)
	{
		OutPath.EmplaceAt(0, *Current);
	}

	// Common room between
	OutPath.Add(Common);

	// Adds the second part of the path (Common -> To)
	Current = &Common;
	while ((Current = ParentsReverse.Find(*Current)) != nullptr)
	{
		OutPath.Add(*Current);
	}
}

// Uses Bidirectional BFS to find a path between A and B
bool UDungeonGraph::FindPath(const URoom* From, const URoom* To, TArray<const URoom*>* OutPath, bool IgnoreLocked)
{
	if (OutPath)
		OutPath->Empty();

	if (!IsValid(From) || !IsValid(To))
		return false;

	// Always path between a room and itself
	if (From == To)
	{
		if (OutPath)
			OutPath->Add(From);
		return true;
	}

	if (!IgnoreLocked && (From->IsLocked() || To->IsLocked()))
		return false;

	// Bidirectional BFS initialization
	TMap<const URoom*, const URoom*> ParentsForward, ParentsReverse;
	TSet<const URoom*> MarkedForward, MarkedReverse; // (visited rooms)
	TQueue<const URoom*> QueueForward, QueueReverse; // (rooms to visit)
	QueueForward.Enqueue(From);
	QueueReverse.Enqueue(To);
	MarkedForward.Add(From);
	MarkedReverse.Add(To);

	// Both are filled when during either cycle an intersection is found
	const URoom* Common = nullptr;

	// Bidirectional BFS
	while (Common == nullptr && !QueueForward.IsEmpty() && !QueueReverse.IsEmpty())
	{
		// BFS from A
		if (!BFS_Cycle(QueueForward, MarkedForward, MarkedReverse, ParentsForward, Common, IgnoreLocked))
		{
			// BFS from B if no common found
			BFS_Cycle(QueueReverse, MarkedReverse, MarkedForward, ParentsReverse, Common, IgnoreLocked);
		}
	}

	// Intersection has been found between MarkedForward and MarkedReverse
	if (Common != nullptr && OutPath != nullptr)
	{
		ReconstructPath(Common, ParentsForward, ParentsReverse, *OutPath);
	}

	return Common != nullptr;
}

void CopyRooms(TArray<URoom*>& To, TArray<URoom*>& From)
{
	for (URoom* Room : From)
	{
		if (Room->Instance)
			DungeonLog_Debug("[%s] Loaded Level: %s", *GetNameSafe(Room), *GetNameSafe(Room->Instance->GetLoadedLevel()));
	}

	To = TArray<URoom*>(From);
}

void UDungeonGraph::SynchronizeRooms()
{
	AActor* Owner = GetTypedOuter<AActor>();
	if (!IsValid(Owner))
		return;

	if (Owner->HasAuthority())
	{
		Owner->FlushNetDormancy();
		RegisterReplicableSubobjects(false);
		CopyRooms(ReplicatedRooms, Rooms);
		RegisterReplicableSubobjects(true);
		MARK_PROPERTY_DIRTY_FROM_NAME(UDungeonGraph, ReplicatedRooms, this);
	}
	else
	{
		CopyRooms(Rooms, ReplicatedRooms);
		RebuildBounds();
		DungeonLog_Debug("Synchronized Rooms from server (length: %d)", Rooms.Num());
		for (const URoom* Room : Rooms)
		{
			DungeonLog_Debug(" - %s (Data: %s Valid: %d)", *GetNameSafe(Room), *GetNameSafe(Room->GetRoomData()), IsValid(Room->GetRoomData()));
		}
	}

	bIsDirty = false;
}

bool UDungeonGraph::AreRoomsLoaded(int32& NbRoomLoaded) const
{
	NbRoomLoaded = 0;
	for (URoom* Room : Rooms)
	{
		if (Room->IsInstanceLoaded())
			NbRoomLoaded++;
	}
	return NbRoomLoaded >= Rooms.Num();
}

bool UDungeonGraph::AreRoomsUnloaded(int32& NbRoomUnloaded) const
{
	NbRoomUnloaded = 0;
	for (URoom* Room : Rooms)
	{
		if (!IsValid(Room) || Room->IsInstanceUnloaded())
			NbRoomUnloaded++;
	}
	return NbRoomUnloaded >= Rooms.Num();
}

bool UDungeonGraph::AreRoomsInitialized(int32& NbRoomInitialized) const
{
	NbRoomInitialized = 0;
	for (URoom* Room : Rooms)
	{
		if (Room->IsInstanceInitialized())
			NbRoomInitialized++;
	}
	return NbRoomInitialized >= Rooms.Num();
}

bool UDungeonGraph::AreRoomsReady() const
{
	for (URoom* Room : Rooms)
	{
		if (!(IsValid(Room) && Room->IsReady()))
			return false;
	}
	return true;
}

void UDungeonGraph::SpawnAllDoors()
{
	// Spawn doors only on server
	// They will be replicated on the clients
	if (!HasAuthority())
		return;

	checkf(Generator.IsValid(), TEXT("Spawning dungeon's doors is only available with a ADungeonGenerator outer."));

	for (auto* RoomConnection : RoomConnections)
	{
		if (RoomConnection->IsDoorInstanced())
			continue;
		RoomConnection->InstantiateDoor(GetWorld(), Generator.Get(), Generator->UseGeneratorTransform());
	}
}

void UDungeonGraph::LoadAllRooms()
{
	// When a level is correct, load all rooms
	for (URoom* Room : Rooms)
	{
		Room->Instantiate(GetWorld());
	}

	SpawnAllDoors();
}

void UDungeonGraph::UnloadAllRooms()
{
	if (HasAuthority())
	{
		for (auto* RoomConnection : RoomConnections)
		{
			ADoor* Door = RoomConnection->GetDoorInstance();
			if (IsValid(Door))
			{
				Door->Destroy();
			}
		}
	}

	for (URoom* Room : Rooms)
	{
		check(Room);
		Room->Destroy();
	}
}

void UDungeonGraph::UpdateBounds(const URoom* Room)
{
	check(IsValid(Room));
	Bounds += Room->GetVoxelBounds();
}

void UDungeonGraph::RebuildBounds()
{
	Bounds = FVoxelBounds();
	for (const URoom* Room : Rooms)
	{
		UpdateBounds(Room);
	}
}

void UDungeonGraph::OnRep_Rooms()
{
	DungeonLog_Debug("Replicated Rooms Changed! (length: %d)", ReplicatedRooms.Num());
	for (int i = 0; i < ReplicatedRooms.Num(); ++i)
	{
		// Trigger Room List Changed only when all received rooms are valid
		if (!IsValid(ReplicatedRooms[i]))
			return;

		DungeonLog_Debug("Replicated Room [%d]: %s", i, *GetNameSafe(ReplicatedRooms[i]));
	}

	DungeonLog_Debug("Trigger Dungeon Reload!");
	MarkDirty();
}
