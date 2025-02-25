/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Benoit Pelletier
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

#include "DungeonGeneratorBase.h"
#include "Engine/Engine.h" // GEngine
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "RoomData.h"
#include "Room.h"
#include "RoomConnection.h"
#include "Door.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGraph.h"
#include "Components/PrimitiveComponent.h"
#include "Utils/ReplicationUtils.h"
#include "ProceduralDungeonCustomVersion.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/StructuredArchive.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "DungeonSaveProxyArchive.h"
#include "Utils/DungeonSaveUtils.h"
#include "DrawDebugHelpers.h"
#include "Utils/CompatUtils.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)
#define SetNetUpdateFrequency(X) NetUpdateFrequency = X
#endif

FArchive& operator<<(FArchive& Ar, FDungeonSaveData& Data)
{
	FStructuredArchiveFromArchive(Ar).GetSlot() << Data;
	return Ar;
}

void operator<<(FStructuredArchiveSlot Slot, FDungeonSaveData& Data)
{
	FStructuredArchive::FRecord DungeonRecord = Slot.EnterRecord();
	DungeonRecord.EnterField(AR_FIELD_NAME("GeneratorId")) << Data.GeneratorId;
	DungeonRecord.EnterField(AR_FIELD_NAME("Data")) << Data.Data;

	if (IsSaving(Slot))
	{
		DungeonLog_Debug("Serialized dungeon into saved data: %s", *Data.GeneratorId.ToString());
	}
	else
	{
		DungeonLog_Debug("Deserialized dungeon from saved data: %d", *Data.GeneratorId.ToString());
	}
}

// Sets default values
ADungeonGeneratorBase::ADungeonGeneratorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SeedType = ESeedType::Random;
	Seed = 123456789; // default Seed
	SeedIncrement = 123456; // default Seed increment
	bUseGeneratorTransform = false;

	bAlwaysRelevant = true;
	bReplicates = true;
	NetPriority = 10.0f;
	SetNetUpdateFrequency(20);
	NetDormancy = ENetDormancy::DORM_DormantAll;

	Graph = CreateDefaultSubobject<UDungeonGraph>(TEXT("Dungeon Rooms"));
	Octree = MakeUnique<FDungeonOctree>(FVector::ZeroVector, HALF_WORLD_MAX);
}

void ADungeonGeneratorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(ADungeonGeneratorBase, Seed, Params);
}

bool ADungeonGeneratorBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	bWroteSomething |= Graph->ReplicateSubobject(Channel, Bunch, RepFlags);
	return bWroteSomething;
}

void ADungeonGeneratorBase::SaveDungeon(FDungeonSaveData& SaveData)
{
	if (!HasAnyFlags(RF_WasLoaded))
	{
		DungeonLog_Error("Dungeon is not saveable: it has been spawned at runtime. Dungeon Generator must be placed in level to have a stable GUID (to make it saveable).");
		return;
	}

	SaveData.GeneratorId = Id;
	SaveData.Data.Reset(0);

	FMemoryWriter MemWriter(SaveData.Data);
	SerializeDungeon(MemWriter);
}

void ADungeonGeneratorBase::LoadDungeon(const FDungeonSaveData& SaveData)
{
	if (SaveData.GeneratorId != Id)
	{
		DungeonLog_Error("Can't load saved data for dungeon generator %s: saved generator mismatch (%s)", *Id.ToString(), *SaveData.GeneratorId.ToString());
		return;
	}

	FMemoryReader MemReader(SaveData.Data);
	SerializeDungeon(MemReader);
}

void ADungeonGeneratorBase::SerializeDungeon(FArchive& Archive)
{
	FDungeonSaveProxyArchive ProxyArchive(Archive);

	TUniquePtr<FArchiveFormatterType> Formatter = CreateArchiveFormatterFromArchive(ProxyArchive,
#if WITH_EDITORONLY_DATA
		bUseJsonSave
#else
		false
#endif
	);
	check(nullptr != Formatter);
	FStructuredArchive StructuredArchive(*Formatter);

	FStructuredArchive::FSlot RootSlot = StructuredArchive.Open();
	FStructuredArchive::FRecord RootRecord = RootSlot.EnterRecord();

	SerializeObject(RootRecord, Archive.IsLoading());
}

void ADungeonGeneratorBase::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
{
	// Set the archive to use the custom version for future compatibility
	Record.GetUnderlyingArchive().UsingCustomVersion(FProceduralDungeonCustomVersion::GUID);

#if false // This is the way to check for an older version that needs to convert some data into new ones.
	const int32 DungeonVersion = Ar.CustomVer(FProceduralDungeonCustomVersion::GUID);
	if (DungeonVersion < FProceduralDungeonCustomVersion::NewVersionThatNeedsDataConversion)
	{
		// Convert the data here
	}
#endif

	if (!bIsLoading)
	{
		DungeonLog_Info("Start Saving Dungeon.");
		IDungeonSaveInterface::DispatchPreSaveEvent(Graph);
	}
	else
	{
		DungeonLog_Info("Start Loading Dungeon.");
	}

	Record.EnterField(AR_FIELD_NAME("Id")) << Id;
	SerializeScriptProperties(Record.EnterField(AR_FIELD_NAME("Properties")));

	FStructuredArchiveRecord GraphRecord = Record.EnterRecord(AR_FIELD_NAME("Graph"));
	SerializeUObject(GraphRecord, Graph, bIsLoading);

	if (bIsLoading)
	{
		checkf(!IsLoading(), TEXT("Dungeon Generator is already loading."));
		EnumAddFlags(Flags, EGeneratorFlags::LoadSavedDungeon);
	}
	else
	{
		DungeonLog_Info("End Saving Dungeon.");
	}
}

void ADungeonGeneratorBase::PostActorCreated()
{
	Super::PostActorCreated();
	if (!Id.IsValid())
	{
		Id = FGuid::NewGuid();
		DungeonLog_Debug("New GUID Generated for DungeonGenerator '%s': %s", *GetName(), *Id.ToString());
	}
	else
	{
		DungeonLog_Debug("Reusing existing GUID for DungeonGenerator '%s': %s", *GetName(), *Id.ToString());
	}
}

void ADungeonGeneratorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Graph->Generator = this;
	Graph->RegisterAsReplicable(true);
}

void ADungeonGeneratorBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(EndPlayReason == EEndPlayReason::Destroyed)
		Graph->UnloadAllRooms();
}

void ADungeonGeneratorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OnStateTick(CurrentState);
}

void ADungeonGeneratorBase::Generate()
{
	// Do it only on server, do nothing on clients
	if (!HasAuthority())
		return;

	EnumAddFlags(Flags, EGeneratorFlags::Generating);
}

void ADungeonGeneratorBase::Unload()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
		return

	// Mark graph as if the room list have been modified.
	// Doing so will not trigger the generation phase,
	// thus the resulting dungeon will be empty.
	Graph->MarkDirty();
}

void ADungeonGeneratorBase::StartNewDungeon()
{
	OnGenerationInit();
	Graph->Clear();
}

void ADungeonGeneratorBase::FinalizeDungeon()
{
	Graph->InitRooms();
	InitializeDungeon(Graph);
}

URoom* ADungeonGeneratorBase::CreateRoomInstance(URoomData* RoomData)
{
	URoom* Instance = NewObject<URoom>(this);
	Instance->Init(RoomData, this, Graph->Count());
	return Instance;
}

bool ADungeonGeneratorBase::TryPlaceRoom(URoom* const& Room, int DoorIndex, const FDoorDef& TargetDoor, const UWorld* World) const
{
	if (!IsValid(Room))
	{
		return false;
	}

	Room->SetPositionAndRotationFromDoor(DoorIndex, TargetDoor.Position, TargetDoor.Direction);

	// Test if it fits in the place
	bool bCanBePlaced = !URoom::Overlap(*Room, Graph->GetAllRooms());
	// @TODO: Should be more performant to use voxel bounds instead of room bounds
	// Also will be mandatory when RoomData will get voxel bounds editor
	// But for now if the RoomUnit is really small (like (1,1,1)) it is really a bottle neck of performence...
	//bool bCanBePlaced = !FVoxelBounds::Overlap(Room->GetVoxelBounds(), Graph->GetVoxelBounds());

	// Check that it does not collide with the world too
	if (bCanBePlaced && bUseWorldCollisionChecks)
	{
		if (!World)
			World = GetWorld();

		const FBoxCenterAndExtent Bounds = Room->GetBounds();
		const FTransform& DungeonTransform = GetDungeonTransform();
		const bool bCollideWithWorld = World->OverlapBlockingTestByChannel(
			DungeonTransform.TransformPositionNoScale(Bounds.Center),
			DungeonTransform.GetRotation(),
			ECC_WorldStatic,
			FCollisionShape::MakeBox(Bounds.Extent),
			WorldCollisionParams
		);
		bCanBePlaced &= !bCollideWithWorld;
	}

	return bCanBePlaced;
}

bool ADungeonGeneratorBase::AddRoomToDungeon(URoom* const& Room, const TArray<int>& DoorsToConnect, bool bFailIfNotConnected)
{
	if (!IsValid(Room))
	{
		// @TODO: do something to be able to call OnFailedToAddRoom here (either pass arguments or change them for current room)
		//OnFailedToAddRoom(ParentRoom.GetRoomData(), doorDef);
		return false;
	}

	bool bConnected = false;
	// Connect the doors if provided, otherwise try to connect all possible doors
	if (DoorsToConnect.Num() <= 0)
	{
		bConnected = Graph->TryConnectToExistingDoors(Room);
	}
	else
	{
		for (int DoorIndex : DoorsToConnect)
		{
			bConnected |= Graph->TryConnectDoor(Room, DoorIndex);
		}
	}

	if (bFailIfNotConnected && !bConnected)
	{
		// @TODO: do something to be able to call OnFailedToAddRoom here (either pass arguments or change them for current room)
		//OnFailedToAddRoom(ParentRoom.GetRoomData(), doorDef);
		return false;
	}

	Graph->AddRoom(Room);
	OnRoomAdded(Room->GetRoomData(), Room);
	return true;
}

// Needed to make this function to have default param for both C++ and Blueprint...
// Could be removed when TOptional will be supported in Blueprint.
bool ADungeonGeneratorBase::AddRoomToDungeon(URoom* const& Room)
{
	return AddRoomToDungeon(Room, {});
}

bool ADungeonGeneratorBase::CreateDungeon_Implementation()
{
	DungeonLog_Error("CreateDungeon is not overriden!");
	return false;
}

void ADungeonGeneratorBase::ChooseDoorClasses()
{
	if (!HasAuthority())
		return;

	if (!IsGenerating())
		return;

	for (auto* Conn : Graph->GetAllConnections())
	{
		check(IsValid(Conn));

		const URoom* RoomA = Conn->GetRoomA().Get();
		const URoom* RoomB = Conn->GetRoomB().Get();

		const URoomData* RoomAData = (IsValid(RoomA)) ? RoomA->GetRoomData() : nullptr;
		const URoomData* RoomBData = (IsValid(RoomB)) ? RoomB->GetRoomData() : nullptr;

		const UDoorType* DoorType = URoomConnection::GetDoorType(Conn);

		bool bFlipped = false;
		TSubclassOf<ADoor> DoorClass = ChooseDoor(RoomAData, RoomA, RoomBData, RoomB, DoorType, bFlipped);
		Conn->SetDoorClass(DoorClass, bFlipped);
	}
}

void ADungeonGeneratorBase::UpdateRoomVisibility()
{
	APawn* Player = GetVisibilityPawn();
	if (!IsValid(Player))
		return;

	// Copied from AActor::GetComponentsBoundingBox but check also collision response with the room object type
	FBox WorldPlayerBox(ForceInit);
	Player->ForEachComponent<UPrimitiveComponent>(/*bIncludeFromChildActors = */false
		, [&](const UPrimitiveComponent* Component)
		{
			if (Component->IsRegistered()
				&& Component->IsCollisionEnabled()
				&& Component->GetCollisionResponseToChannel(Dungeon::RoomObjectType()) != ECollisionResponse::ECR_Ignore
				)
			{
				WorldPlayerBox += Component->Bounds.GetBox();
			}
		});

	FTransform Transform = UseGeneratorTransform() ? GetTransform() : FTransform::Identity;
	WorldPlayerBox = WorldPlayerBox.InverseTransformBy(Transform);

	TSet<URoom*> RoomsToHide(CurrentPlayerRooms);
	CurrentPlayerRooms.Empty();
	FindElementsWithBoundsTest(*Octree, WorldPlayerBox, [this, &RoomsToHide](const FDungeonOctreeElement& Element)
	{
		RoomsToHide.Remove(Element.Room);
		CurrentPlayerRooms.Add(Element.Room);
		Element.Room->SetPlayerInside(true);
	});

	for (URoom* room : RoomsToHide)
	{
		room->SetPlayerInside(false);
	}

	const bool bIsOcclusionEnabled = Dungeon::OcclusionCulling();
	const uint32 OcclusionDistance = Dungeon::OcclusionDistance();

	// Detects occlusion setting changes and toggles on/off all room visibilities when occlusion is enabled/disabled.
	if (bWasOcclusionEnabled != bIsOcclusionEnabled
		|| PreviousOcclusionDistance != OcclusionDistance)
	{
		RoomsToHide.Empty();
		for (URoom* Room : Graph->GetAllRooms())
		{
			Room->SetVisible(!bIsOcclusionEnabled);
		}
	}
	bWasOcclusionEnabled = bIsOcclusionEnabled;
	PreviousOcclusionDistance = OcclusionDistance;

	// Don't change room visibilities if occlusion is disabled.
	if (!bIsOcclusionEnabled)
		return;

	TSet<URoom*> VisibleRooms;
	UDungeonGraph::TraverseRooms(CurrentPlayerRooms, &VisibleRooms, OcclusionDistance, [](URoom* room) { room->SetVisible(true); });
	UDungeonGraph::TraverseRooms(RoomsToHide, nullptr, OcclusionDistance, [&VisibleRooms](URoom* room) { room->SetVisible(VisibleRooms.Contains(room)); });
}

void ADungeonGeneratorBase::Reset()
{
	CurrentPlayerRooms.Empty();
	Octree->Destroy();
}

void ADungeonGeneratorBase::UpdateOctree()
{
	Octree->Destroy();
	for (URoom* r : Graph->GetAllRooms())
	{
		check(IsValid(r));
		FBoxCenterAndExtent bounds = r->GetBounds();
		FDungeonOctreeElement octreeElement(r);
		Octree->AddElement(octreeElement);
		r->SetVisible(false);
	}
}

void ADungeonGeneratorBase::UpdateSeed()
{
	switch (SeedType)
	{
	case ESeedType::Random:
		Random.GenerateNewSeed();
		Seed = Random.GetCurrentSeed();
		break;
	case ESeedType::AutoIncrement:
		if(bShouldIncrement)
			Seed += SeedIncrement;
		else
			bShouldIncrement = true;
		// no break so we initialize the seed too
	case ESeedType::Fixed:
		Random.Initialize(Seed);
		break;
	default:
		checkNoEntry();
		break;
	}

	DungeonLog_Info("Seed: %d", Seed);
}

// @TODO: Place the debug draw in an editor module of the plugin?
void ADungeonGeneratorBase::DrawDebug() const
{
#if ENABLE_DRAW_DEBUG
	if (!Dungeon::DrawDebug())
		return;

#if WITH_EDITORONLY_DATA
	if (!bDrawDebugDungeonBounds)
		return;
#endif

	const FTransform& Transform = GetDungeonTransform();
	FBoxCenterAndExtent DungeonBounds = Graph->GetDungeonBounds(Transform);
	DrawDebugBox(GetWorld(), DungeonBounds.Center, DungeonBounds.Extent, Transform.GetRotation(), FColor::Yellow);
#endif
}

/*
 *	=======================================
 *				State Machine
 *	=======================================
 */
void ADungeonGeneratorBase::SetState(EGenerationState NewState)
{
	OnStateEnd(CurrentState);
	CurrentState = NewState;
	OnStateBegin(CurrentState);
}

void ADungeonGeneratorBase::OnStateBegin(EGenerationState State)
{
	CachedTmpRoomCount = 0;
	switch (State)
	{
	case EGenerationState::Unload:
		DungeonLog_Info("======= Begin Unload All Levels =======");
		Reset();
		DungeonLog_Info("Nb Room To Unload: %d", Graph->Count());
		Graph->UnloadAllRooms();
		break;
	case EGenerationState::Generation:
		DungeonLog_Info("======= Begin Dungeon Generation =======");
		check(HasAuthority()); // should never generate on clients!
		FlushNetDormancy();
		UpdateSeed();
		if (!CreateDungeon())
		{
			Graph->Clear();
			OnGenerationFailed();
		}
		break;
	case EGenerationState::Initialization:
		DungeonLog_Info("======= Begin Dungeon Initialization =======");

		if (IsLoadingSavedDungeon())
		{
			Graph->RetrieveRoomsFromLoadedData();
		}

		Graph->SynchronizeRooms();
		break;
	case EGenerationState::Load:
		DungeonLog_Info("======= Begin Load All Levels =======");
		DungeonLog_Info("Nb Room To Load: %d", Graph->Count());
		// I've placed `ChooseDoor` here to keep same behavior as before,
		// but it could be moved during the generation step in a future version
		// (e.g. in the `FinalizeDungeon` function)
		ChooseDoorClasses();
		Graph->LoadAllRooms();
		break;
	case EGenerationState::Idle:
		DungeonLog_Info("======= Ready To Play =======");
		break;
	default:
		checkNoEntry();
		break;
	}
}

void ADungeonGeneratorBase::OnStateTick(EGenerationState State)
{
	switch (State)
	{
	case EGenerationState::Idle:
		UpdateRoomVisibility();
		DrawDebug();
		if (Graph->IsDirty() || IsGenerating() || IsLoadingSavedDungeon())
			SetState(EGenerationState::Unload);
		break;
	case EGenerationState::Unload:
		if (Graph->AreRoomsUnloaded(CachedTmpRoomCount))
			SetState((HasAuthority() && IsGenerating()) ? EGenerationState::Generation : EGenerationState::Initialization);
		break;
	case EGenerationState::Generation:
		SetState(EGenerationState::Initialization);
		break;
	case EGenerationState::Initialization:
		if (Graph->AreRoomsReady())
			SetState(EGenerationState::Load);
		break;
	case EGenerationState::Load:
		if (Graph->AreRoomsInitialized(CachedTmpRoomCount))
			SetState(EGenerationState::Idle);
		break;
	default:
		break;
	}
}

void ADungeonGeneratorBase::OnStateEnd(EGenerationState State)
{
	UNavigationSystemV1* nav = nullptr;
	switch (State)
	{
	case EGenerationState::Idle:
		OnPreGeneration();

		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav && bRebuildNavmesh)
		{
			// Lock navmesh rebuild, so we don't trigger a rebuild for each room loaded/unloaded
			DungeonLog_Debug("Lock navmesh update/rebuild");
			nav->AddNavigationBuildLock(ENavigationBuildLock::Custom);
		}
		break;
	case EGenerationState::Unload:
		if (HasAuthority())
			Graph->Clear();
		GetWorld()->FlushLevelStreaming();
		GEngine->ForceGarbageCollection(true);
		DungeonLog_Info("======= End Unload All Levels =======");
		break;
	case EGenerationState::Generation:
		DungeonLog_Info("======= End Dungeon Generation =======");
		break;
	case EGenerationState::Initialization:
		DungeonLog_Info("======= End Dungeon Initialization =======");
		UpdateOctree();
		break;
	case EGenerationState::Load:
		DungeonLog_Info("======= End Load All Levels =======");

		if (IsLoadingSavedDungeon())
		{
			IDungeonSaveInterface::DispatchPostLoadEvent(Graph);
			DungeonLog_Info("End Loading Dungeon.");
		}

		EnumRemoveFlags(Flags, EGeneratorFlags::Generating | EGeneratorFlags::LoadSavedDungeon);

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav && bRebuildNavmesh)
		{
			DungeonLog_Info("Rebuild navmesh");

			nav->RemoveNavigationBuildLock(ENavigationBuildLock::Custom);

			// With a dynamic navmesh, we don't need anymore to call Build explicitly
			// Moreover, removing the build lock triggers a rebuild itself.
			nav->CancelBuild();
			nav->Build();
		}

		// Invoke Post Generation Event when initialization is done
		OnPostGeneration();
		break;
	default:
		break;
	}
}

// ===== Default Native Events Implementations =====

TSubclassOf<ADoor> ADungeonGeneratorBase::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoom* CurrentRoomInstance, const URoomData* NextRoom, const URoom* NextRoomInstance, const UDoorType* DoorType, bool& Flipped)
{
	DungeonLog_Error("Error: ChooseDoor not implemented");
	return nullptr;
}

void ADungeonGeneratorBase::InitializeDungeon_Implementation(const UDungeonGraph* Rooms)
{
}

APawn* ADungeonGeneratorBase::GetVisibilityPawn_Implementation()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(Controller))
		return nullptr;

	return Controller->GetPawnOrSpectator();
}

void ADungeonGeneratorBase::OnPreGeneration_Implementation()
{
	OnPreGenerationEvent.Broadcast();
}

void ADungeonGeneratorBase::OnPostGeneration_Implementation()
{
	OnPostGenerationEvent.Broadcast();
}

void ADungeonGeneratorBase::OnGenerationInit_Implementation()
{
	OnGenerationInitEvent.Broadcast();
}

void ADungeonGeneratorBase::OnGenerationFailed_Implementation()
{
	OnGenerationFailedEvent.Broadcast();
}

void ADungeonGeneratorBase::OnRoomAdded_Implementation(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance)
{
	OnRoomAddedEvent.Broadcast(NewRoom, RoomInstance);
}

void ADungeonGeneratorBase::OnFailedToAddRoom_Implementation(const URoomData* FromRoom, const FDoorDef& FromDoor)
{
	OnFailedToAddRoomEvent.Broadcast(FromRoom, FromDoor);
}

// ===== Utility Functions =====

URoomData* ADungeonGeneratorBase::GetRandomRoomData(TArray<URoomData*> RoomDataArray)
{
	if (RoomDataArray.Num() <= 0)
		return nullptr;

	int n = Random.RandRange(0, RoomDataArray.Num() - 1);
	return RoomDataArray[n];
}

URoomData* ADungeonGeneratorBase::GetRandomRoomDataWeighted(const TMap<URoomData*, int>& RoomDataWeightedMap)
{
	if (RoomDataWeightedMap.Num() <= 0)
		return nullptr;

	int Total = Dungeon::GetTotalWeight(RoomDataWeightedMap);
	const int Chosen = Random.RandRange(0, Total - 1);
	return Dungeon::GetWeightedAt(RoomDataWeightedMap, Chosen);
}

void ADungeonGeneratorBase::GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData)
{
	for (URoomData* RoomData : RoomDataArray)
	{
		if (RoomData->HasCompatibleDoor(DoorData))
		{
			CompatibleRooms.Add(RoomData);
			bSuccess = true;
		}
	}
}

float ADungeonGeneratorBase::GetProgress() const
{
	const int32 TotalRoom = Graph->Count();
	switch (CurrentState)
	{
	case EGenerationState::Unload:
		return (TotalRoom > 0)
			? 0.5f * (static_cast<float>(CachedTmpRoomCount) / TotalRoom)
			: 0.0f;
	case EGenerationState::Generation:
	case EGenerationState::Initialization:
		return 0.5f;
	case EGenerationState::Load:
		return (TotalRoom > 0)
			? 0.5f + 0.5f * (static_cast<float>(CachedTmpRoomCount) / TotalRoom)
			: 0.5f;
	default:
		return 1.0f;
	}
}

URoom* ADungeonGeneratorBase::GetRoomByIndex(int64 Index) const
{
	return Graph->GetRoomByIndex(Index);
}

void ADungeonGeneratorBase::SaveAllDungeons(const UObject* WorldContextObject, TArray<FDungeonSaveData>& SavedData)
{
	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (!IsValid(World))
	{
		DungeonLog_Error("Can't save dungeons: no world provided!");
		return;
	}
	TArray<ADungeonGeneratorBase*> Generators;
	WorldUtils::FindAllActors(World, Generators);

	SavedData.SetNum(Generators.Num());

	for (int i = 0; i < Generators.Num(); ++i)
	{
		if (!IsValid(Generators[i]))
		{
			DungeonLog_WarningSilent("One of the dungeon is invalid.");
			continue;
		}

		DungeonLog_InfoSilent("Saving Dungeon %s", *Generators[i]->Id.ToString());
		Generators[i]->SaveDungeon(SavedData[i]);
	}
}

void ADungeonGeneratorBase::LoadAllDungeons(const UObject* WorldContextObject, const TArray<FDungeonSaveData>& SavedData)
{
	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (!IsValid(World))
	{
		DungeonLog_Error("Can't load dungeons: no world provided!");
		return;
	}

	TMap<FGuid, ADungeonGeneratorBase*> Generators;
	WorldUtils::MapActors<FGuid, ADungeonGeneratorBase>(World, Generators, [](const ADungeonGeneratorBase* Generator) { return Generator->Id; });

	for (const auto& SavedDatum : SavedData)
	{
		auto* FoundGenerator = Generators.Find(SavedDatum.GeneratorId);
		if (nullptr == FoundGenerator)
		{
			DungeonLog_WarningSilent("Generator with ID [%s] does not exists.", *SavedDatum.GeneratorId.ToString());
			continue;
		}

		ADungeonGeneratorBase* Generator = *FoundGenerator;
		if (!IsValid(Generator))
		{
			DungeonLog_WarningSilent("Generator with ID [%s] is invalid.", *SavedDatum.GeneratorId.ToString());
			continue;
		}

		DungeonLog_InfoSilent("Loading Dungeon %s", *Generator->Id.ToString());
		Generator->LoadDungeon(SavedDatum);
	}
}

void ADungeonGeneratorBase::SetSeed(int32 NewSeed)
{
	Seed = static_cast<uint32>(NewSeed);
	bShouldIncrement = false; // avoid incrementing when SeedType is AutoIncrement
}

int32 ADungeonGeneratorBase::GetSeed() const
{
	return static_cast<int32>(Seed);
}

FVector ADungeonGeneratorBase::GetDungeonOffset() const
{
	return UseGeneratorTransform() ? GetActorLocation() : FVector::ZeroVector;
}

FQuat ADungeonGeneratorBase::GetDungeonRotation() const
{
	return UseGeneratorTransform() ? GetActorQuat() : FQuat::Identity;
}

const FTransform& ADungeonGeneratorBase::GetDungeonTransform() const
{
	return UseGeneratorTransform() ? GetActorTransform() : FTransform::Identity;
}

// ##############################
//        CONSOLE COMMANDS
// ##############################

// An helper class for console commands
struct FDungeonConsoleCommands
{
private:
	// Fills OutGenerators array with DungeonGenerator actors patching the NameOrTag provided, or all if NameOrTag is empty.
	static bool CollectDungeonGenerators(const TCHAR* CommandName, UWorld* InWorld, const FString& NameOrTag, TArray<ADungeonGeneratorBase*>& OutGenerators);

public:
	// Execute a Callback on all DungeonGenerator actors found depending on the InParams provided.
	static void ExecuteOnDungeonGenerators(const TCHAR* CommandName, const TArray<FString>& InParams, UWorld* InWorld, TFunction<void(ADungeonGeneratorBase*)> Callback);

public:
	static const TCHAR* GenerateCommandName;
	static const TCHAR* UnloadCommandName;
};

const TCHAR* FDungeonConsoleCommands::GenerateCommandName = TEXT("pd.Generate");
const TCHAR* FDungeonConsoleCommands::UnloadCommandName = TEXT("pd.Unload");

FAutoConsoleCommandWithWorldAndArgs CCmdGenerateDungeon(FDungeonConsoleCommands::GenerateCommandName
	, TEXT("Call \"Generate\" on DungeonsGenerator actors with the name or tag provided, or all if nothing provided. USAGE: pd.Generate [name|tag]")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& InParams, UWorld* InWorld) {
		FDungeonConsoleCommands::ExecuteOnDungeonGenerators(FDungeonConsoleCommands::GenerateCommandName
			, InParams
			, InWorld
			, [](ADungeonGeneratorBase* Generator) { Generator->Generate(); }
		);
		})
	, EConsoleVariableFlags::ECVF_Cheat
);

FAutoConsoleCommandWithWorldAndArgs CCmdUnloadDungeon(FDungeonConsoleCommands::UnloadCommandName
	, TEXT("Call \"Unload\" on DungeonsGenerator actors with the name or tag provided, or all if nothing provided. USAGE: pd.Unload [name|tag]")
	, FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& InParams, UWorld* InWorld) {
		FDungeonConsoleCommands::ExecuteOnDungeonGenerators(FDungeonConsoleCommands::UnloadCommandName
			, InParams
			, InWorld
			, [](ADungeonGeneratorBase* Generator) { Generator->Unload(); }
		);
		})
	, EConsoleVariableFlags::ECVF_Cheat
);

bool FDungeonConsoleCommands::CollectDungeonGenerators(const TCHAR* CommandName, UWorld* InWorld, const FString& NameOrTag, TArray<ADungeonGeneratorBase*>& OutGenerators)
{
	if (!IsValid(InWorld) || !InWorld->IsGameWorld())
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("[%s] Invalid world."), CommandName);
		return false;
	}

	OutGenerators.Empty();
	if (!NameOrTag.IsEmpty())
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Search for DungeonGenerator actors with name or tag '%s'."), CommandName, *NameOrTag);
		FString Name(NameOrTag);
		FName Tag(NameOrTag);
		WorldUtils::FindAllActorsByPredicate<ADungeonGeneratorBase>(InWorld, OutGenerators, [&Name, &Tag](const ADungeonGeneratorBase* Generator) { return Generator->GetName() == Name || Generator->ActorHasTag(Tag); });
	}
	else
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Search for all DungeonGenerator actors."), CommandName);
		WorldUtils::FindAllActors(InWorld, OutGenerators);
	}

	// Found no generator
	if (OutGenerators.Num() <= 0)
	{
		UE_LOG(LogProceduralDungeon, Error, TEXT("[%s] No DungeonGenerator found%s.")
			, CommandName
			, (!NameOrTag.IsEmpty())
			? *FString::Printf(TEXT(" with name or tag '%s'"), *NameOrTag)
			: TEXT("")
		);
		return false;
	}

	return true;
}

void FDungeonConsoleCommands::ExecuteOnDungeonGenerators(const TCHAR* CommandName, const TArray<FString>& InParams, UWorld* InWorld, TFunction<void(ADungeonGeneratorBase*)> Callback)
{
	UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Begin."), CommandName);

	TArray<ADungeonGeneratorBase*> Generators;
	if (!CollectDungeonGenerators(CommandName, InWorld, (InParams.Num() > 0) ? InParams[0] : FString(), Generators))
		return;

	for (ADungeonGeneratorBase* Generator : Generators)
	{
		if (!IsValid(Generator))
		{
			UE_LOG(LogProceduralDungeon, Warning, TEXT("[%s] Generator is invalid!."), CommandName);
			continue;
		}

		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Execute command on DungeonGenerator '%s'."), CommandName, *Generator->GetName());
		Callback(Generator);
	}
	UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] End."), CommandName);
}
