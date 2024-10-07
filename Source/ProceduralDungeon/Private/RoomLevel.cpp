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

#include "RoomLevel.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Pawn.h"
#include "ProceduralDungeonTypes.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "Room.h"
#include "RoomData.h"
#include "Door.h"
#include "DungeonGenerator.h"
#include "Components/BoxComponent.h"
#include "RoomVisibilityComponent.h"
#include "RoomVisitor.h"

ARoomLevel::ARoomLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsInit = false;
	Room = nullptr;
	DungeonTransform = FTransform::Identity;

	// Create a root component to have a world position
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Root")));
}

// Use this for initialization
void ARoomLevel::Init(URoom* _Room)
{
	check(IsValid(_Room));
	Room = _Room;
	bIsInit = false;

	DungeonTransform = Room->Generator()->GetDungeonTransform();

	// Update the room's bounding box for occlusion culling (also the red box drawn in debug)
	UpdateBounds();
}

void ARoomLevel::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(Room))
	{
		DungeonLog_Warning("RoomLevel was not spawned by a DungeonGenerator. It's fine for testing a room but occlusion will not work properly. Consider unchecking \"Occlude Dynamic Actors\" in the plugin's settings.");
		return;
	}

	// Check if the data that spawned this level correspond to the data provided in blueprint
	if (Data != Room->GetRoomData())
	{
		DungeonLog_Error("RoomLevel's Data does not match RoomData's Level [Data \"%s\" | Level \"%s\"]. Debug Draw will be incorrect.", *GetNameSafe(Room->GetRoomData()), *GetName());
	}

	// Create trigger box to track dynamic actors inside the room with IRoomVisitor
	RoomTrigger = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Room Trigger"));
	RoomTrigger->RegisterComponent();
	RoomTrigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RoomTrigger->SetCanEverAffectNavigation(false);
	RoomTrigger->SetGenerateOverlapEvents(true);
	RoomTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RoomTrigger->SetCollisionObjectType(Dungeon::RoomObjectType());
	RoomTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RoomTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	RoomTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	RoomTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	static const uint8 FirstChannel = static_cast<uint8>(ECollisionChannel::ECC_GameTraceChannel1);
	static const uint8 LastChannel = static_cast<uint8>(ECollisionChannel::ECC_GameTraceChannel18);
	for (uint8 Channel = FirstChannel; Channel <= LastChannel; ++Channel)
	{
		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(static_cast<ECollisionChannel>(Channel));
		if (TraceChannel != ETraceTypeQuery::TraceTypeQuery_MAX)
		{
			RoomTrigger->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(Channel), ECollisionResponse::ECR_Ignore);
		}
	}

	RoomTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoomLevel::OnTriggerBeginOverlap);
	RoomTrigger->OnComponentEndOverlap.AddDynamic(this, &ARoomLevel::OnTriggerEndOverlap);

	// Update trigger box to have the room's bounds
	FBoxCenterAndExtent LocalBounds = Room->GetLocalBounds();
	RoomTrigger->SetRelativeLocationAndRotation(LocalBounds.Center, FQuat::Identity);
	RoomTrigger->SetBoxExtent(LocalBounds.Extent, true);

	SetActorsVisible(Room->IsVisible());

	// Create dynamic components from the RoomCustomData
	Room->CreateLevelComponents(this);

	bIsInit = true;
}

void ARoomLevel::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Room = nullptr;
}

// Update is called once per frame
void ARoomLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	// @TODO: Place the debug draw in an editor module of the plugin?
	if (Dungeon::DrawDebug() && IsValid(Data))
	{
		const bool bIsRoomValid = (Room != nullptr);
		const bool bIsRoomDataValid = bIsRoomValid && (Data == Room->GetRoomData());

		const FTransform& RoomTransform = (bIsRoomValid) ? Room->GetTransform() : FTransform::Identity;
		const bool bIsRoomLocked = bIsRoomValid && Room->IsLocked();
		UpdateBounds();

		// Cache world
		const UWorld* World = GetWorld();

		// @TODO: is it still needed now?
		// Pivot
		if(Dungeon::ShowRoomOrigin())
			DrawDebugSphere(World, DungeonTransform.TransformPositionNoScale(RoomTransform.GetLocation()), 100.0f, 4, FColor::Magenta);

		// Room bounds
		DrawDebugBox(World, GetBoundsCenter(), GetBoundsExtent(), DungeonTransform.GetRotation(), IsPlayerInside() ? FColor::Green : FColor::Red);

		if (bIsRoomLocked)
		{
			FBox Box = Bounds.GetBox();
			const FVector& Min = Box.Min;
			const FVector& Max = Box.Max;
#ifdef T
			static_assert(false, "T macro is already defined! Please change its name to avoid potential conflicts");
#endif
#define T(POINT) DungeonTransform.TransformPositionNoScale(POINT)
			DrawDebugLine(World, T(Min), T(Max), FColor::Red);
			DrawDebugLine(World, T(FVector(Min.X, Min.Y, Max.Z)), T(FVector(Max.X, Max.Y, Min.Z)), FColor::Red);
			DrawDebugLine(World, T(FVector(Min.X, Max.Y, Max.Z)), T(FVector(Max.X, Min.Y, Min.Z)), FColor::Red);
			DrawDebugLine(World, T(FVector(Min.X, Max.Y, Min.Z)), T(FVector(Max.X, Min.Y, Max.Z)), FColor::Red);
#undef T
		}

		// Doors
		for (int i = 0; i < Data->GetNbDoor(); i++)
		{
			const bool bIsConnected = !bIsRoomValid || (bIsRoomDataValid && Room->IsConnected(i));
			const bool bIsDoorValid = Data->IsDoorValid(i) && !Data->IsDoorDuplicate(i);
			FDoorDef::DrawDebug(World, bIsDoorValid ? FColor::Blue : FColor::Orange, Data->Doors[i], RoomTransform * DungeonTransform, true, bIsConnected);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

bool ARoomLevel::IsPlayerInside()
{
	return IsValid(Room) ? Room->IsPlayerInside() : false;
}

bool ARoomLevel::IsVisible()
{
	return IsValid(Room) ? Room->IsVisible() : true;
}

bool ARoomLevel::IsLocked()
{
	return IsValid(Room) ? Room->IsLocked() : false;
}

void ARoomLevel::Lock(bool lock)
{
	if(IsValid(Room))
		Room->Lock(lock);
}

void ARoomLevel::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TriggerActor(OtherActor, true);
}

void ARoomLevel::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TriggerActor(OtherActor, false);
}

FVector ARoomLevel::GetBoundsCenter() const
{
	return DungeonTransform.TransformPositionNoScale(Bounds.Center);
}

FVector ARoomLevel::GetBoundsExtent() const
{
	return Bounds.Extent;
}

void ARoomLevel::UpdateBounds()
{
	if (IsValid(Room))
	{
		Bounds = Room->GetBounds();
	}
	else if (IsValid(Data))
	{
		Bounds = Data->GetBounds();
	}
}

void ARoomLevel::SetActorsVisible(bool Visible)
{
	if (!Dungeon::OcclusionCulling())
	{
		// Force visibility when occlusion culling is disabled
		Visible = true;
	}

	ULevel* Level = GetLevel();
	if (IsValid(Level))
	{
		for (AActor* Actor : Level->Actors)
		{
			if (!IsValid(Actor))
				continue;

			// HACK: Don't manage replicated actors as their ActorHiddenInGame is replicated
			// and will mess up the actor visibility on clients!
			if (Actor->GetIsReplicated())
				continue;

			// Discard explicitly ignored actors.
			// They can have a (Static) Room Visibility Component attached to have a custom occlusion management.
			if (Actor->ActorHasTag(FName("Ignore Room Culling")))
				continue;

			Actor->SetActorHiddenInGame(!Visible);
		}
	}

	// Notify the change (useful for RoomVisibilityComponent)
	VisibilityChangedEvent.Broadcast(this, Visible);
}

void ARoomLevel::UpdateVisitor(UObject* Visitor, bool IsInside)
{
	check(Visitor->Implements<URoomVisitor>());

	if (IsInside && !Visitors.Contains(Visitor))
	{
		Visitors.Add(Visitor);
		IRoomVisitor::Execute_OnRoomEnter(Visitor, this);
	}
	else if (!IsInside && Visitors.Contains(Visitor))
	{
		Visitors.Remove(Visitor);
		IRoomVisitor::Execute_OnRoomExit(Visitor, this);
	}
}

void ARoomLevel::TriggerActor(AActor* Actor, bool IsInTrigger)
{
	if (!IsValid(Actor))
		return;

	// Call the interface on the actor itself
	if (Actor->Implements<URoomVisitor>())
	{
		UpdateVisitor(Actor, IsInTrigger);
	}

	// Call the interface on its components too
	TArray<UActorComponent*, FDefaultAllocator> VisitorComps = Actor->GetComponentsByInterface(URoomVisitor::StaticClass());
	for (UActorComponent* VisitorComp : VisitorComps)
	{
		check(VisitorComp);
		UpdateVisitor(VisitorComp, IsInTrigger);
	}

	if (IsInTrigger)
		ActorEnterRoomEvent.Broadcast(this, Actor);
	else
		ActorExitRoomEvent.Broadcast(this, Actor);
}

void ARoomLevel::PostInitProperties()
{
	Super::PostInitProperties();
	UpdateBounds();
}

#if WITH_EDITOR
void ARoomLevel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateBounds();
	OnPropertiesChanged.Broadcast(this);
}
#endif
