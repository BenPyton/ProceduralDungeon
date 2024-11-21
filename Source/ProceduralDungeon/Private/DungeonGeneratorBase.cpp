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

#include "DungeonGeneratorBase.h"
#include "Engine/Engine.h" // GEngine
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "RoomData.h"
#include "Room.h"
#include "Door.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "DungeonGraph.h"
#include "Components/PrimitiveComponent.h"
#include "Utils/ReplicationUtils.h"

uint32 ADungeonGeneratorBase::GeneratorCount = 0;

#if UE_VERSION_OLDER_THAN(5, 5, 0)
#define SetNetUpdateFrequency(X) NetUpdateFrequency = X
#endif

// Sets default values
ADungeonGeneratorBase::ADungeonGeneratorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SeedType = ESeedType::Random;
	Seed = 123456789; // default Seed
	SeedIncrement = 123456; // default Seed increment
	UniqueId = GeneratorCount++; // TODO: make it better than a static increment. It can be increased very quickly in editor when we move an actor.
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
	DOREPLIFETIME_WITH_PARAMS(ADungeonGeneratorBase, Generation, Params);
}

bool ADungeonGeneratorBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	bWroteSomething |= Graph->ReplicateSubobject(Channel, Bunch, RepFlags);
	return bWroteSomething;
}

void ADungeonGeneratorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Graph->Generator = this;
	Graph->RegisterAsReplicable(true);
}

void ADungeonGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADungeonGeneratorBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(EndPlayReason == EEndPlayReason::Destroyed)
		UnloadAllRooms();
}

void ADungeonGeneratorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OnStateTick(CurrentState);
}

void ADungeonGeneratorBase::Generate()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
	{
		Graph->RequestGeneration();
	}
}

void ADungeonGeneratorBase::Unload()
{
	// Do it only on server, do nothing on clients
	if (HasAuthority())
	{
		Graph->RequestUnload();
	}
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
	bool bCanBePlaced = !URoom::Overlap(*Room, Graph->Rooms);

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
		bConnected = Room->TryConnectToExistingDoors(Graph->GetAllRooms());
	}
	else
	{
		for (int DoorIndex : DoorsToConnect)
		{
			bConnected |= Room->TryConnectDoor(DoorIndex, Graph->GetAllRooms());
		}
	}

	if (bFailIfNotConnected && !bConnected)
	{
		// @TODO: do something to be able to call OnFailedToAddRoom here (either pass arguments or change them for current room)
		//OnFailedToAddRoom(ParentRoom.GetRoomData(), doorDef);
		return false;
	}

	Graph->Rooms.Add(Room);
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

void ADungeonGeneratorBase::InstantiateRoom(URoom* Room)
{
	// Instantiate room
	Room->Instantiate(GetWorld());

	// Spawn only doors on server
	if (!HasAuthority())
		return;

	for (int i = 0; i < Room->GetConnectionCount(); i++)
	{
		// Get next room
		URoom* r = Room->GetConnection(i).Get();
		FIntVector DoorCell = Room->GetDoorWorldPosition(i);
		EDoorDirection DoorRot = Room->GetDoorWorldOrientation(i);
		int j = Room->GetOtherDoorIndex(i);

		// Don't instantiate door if it's the parent
		if (!Room->IsDoorInstanced(i))
		{
			bool Flipped = false;
			TSubclassOf<ADoor> DoorClass = ChooseDoor(Room->GetRoomData(), nullptr != r ? r->GetRoomData() : nullptr, Room->GetRoomData()->Doors[i].Type, Flipped);

			if (nullptr != DoorClass)
			{
				FVector InstanceDoorPos = GetDungeonRotation().RotateVector(FDoorDef::GetRealDoorPosition(DoorCell, DoorRot)) + GetDungeonOffset();
				FQuat InstanceDoorRot = GetDungeonRotation() * FRotator(0, 90 * (int8)DoorRot + Flipped * 180, 0).Quaternion();
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ADoor* Door = GetWorld()->SpawnActor<ADoor>(DoorClass, InstanceDoorPos, InstanceDoorRot.Rotator(), SpawnParams);

				if (IsValid(Door))
				{
					DoorList.Add(Door);
					Door->SetConnectingRooms(Room, r);
					Room->SetDoorInstance(i, Door);
					if (IsValid(r))
					{
						r->SetDoorInstance(j, Door);
					}
				}
				else
				{
					DungeonLog_Error("Failed to spawn Door, make sure you set door actor to always spawning.");
				}
			}
		}
	}
}

void ADungeonGeneratorBase::LoadAllRooms()
{
	// When a level is correct, load all rooms
	for (URoom* Room : Graph->GetAllRooms())
	{
		InstantiateRoom(Room);
	}
}

void ADungeonGeneratorBase::UnloadAllRooms()
{
	if (HasAuthority())
	{
		for (ADoor* Door : DoorList)
		{
			Door->Destroy();
		}
		DoorList.Empty();
	}

	for (URoom* Room : Graph->GetAllRooms())
	{
		check(Room);
		Room->Destroy();
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
	for (URoom* r : Graph->Rooms)
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
		UnloadAllRooms();
		break;
	case EGenerationState::Generation:
		DungeonLog_Info("======= Begin Dungeon Generation =======");
		check(HasAuthority()); // should never generate on clients!
		FlushNetDormancy();
		++Generation;
		UpdateSeed();
		if (!CreateDungeon())
		{
			Graph->Clear();
			OnGenerationFailed();
		}
		break;
	case EGenerationState::Initialization:
		DungeonLog_Info("======= Begin Dungeon Initialization =======");
		Graph->SynchronizeRooms();
		break;
	case EGenerationState::Load:
		DungeonLog_Info("======= Begin Load All Levels =======");
		DungeonLog_Info("Nb Room To Load: %d", Graph->Count());
		LoadAllRooms();
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
		if (Graph->IsDirty())
			SetState(EGenerationState::Unload);
		break;
	case EGenerationState::Unload:
		if (Graph->AreRoomsUnloaded(CachedTmpRoomCount))
			SetState((HasAuthority() && Graph->IsRequestingGeneration()) ? EGenerationState::Generation : EGenerationState::Initialization);
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
		if (nullptr != nav)
		{
			// Lock navmesh rebuild, so we don't trigger a rebuild for each room loaded/unloaded
			DungeonLog_Info("Lock navmesh update/rebuild");
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

		// Try to rebuild the navmesh
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
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

TSubclassOf<ADoor> ADungeonGeneratorBase::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped)
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

void ADungeonGeneratorBase::SetSeed(int32 NewSeed)
{
	Seed = static_cast<uint32>(NewSeed);
	bShouldIncrement = false; // avoid incrementing when SeedType is AutoIncrement
}

int32 ADungeonGeneratorBase::GetSeed()
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
		World::FindAllActorsByPredicate<ADungeonGeneratorBase>(InWorld, OutGenerators, [&Name, &Tag](const ADungeonGeneratorBase* Generator) { return Generator->GetName() == Name || Generator->ActorHasTag(Tag); });
	}
	else
	{
		UE_LOG(LogProceduralDungeon, Log, TEXT("[%s] Search for all DungeonGenerator actors."), CommandName);
		World::FindAllActors(InWorld, OutGenerators);
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
