/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Benoit Pelletier
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
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Pawn.h"
#include "ProceduralDungeonTypes.h"
#include "ProceduralDungeonLog.h"
#include "Room.h"
#include "RoomData.h"
#include "Door.h"
#include "DungeonGenerator.h"
#include "Components/BoxComponent.h"
#include "RoomVisibilityComponent.h"

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

	DungeonTransform.SetLocation(Room->Generator()->GetDungeonOffset());
	DungeonTransform.SetRotation(Room->Generator()->GetDungeonRotation());

	// Update the room's bounding box for occlusion culling (also the red box drawn in debug)
	UpdateBounds();
}

void ARoomLevel::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(Room))
	{
		LogWarning(TEXT("RoomLevel was not spawned by a DungeonGenerator. It's fine for testing a room but occlusion will not work properly. Consider unchecking \"Occlude Dynamic Actors\" in the plugin's settings."));
		return;
	}

	// Check if the data that spawned this level correspond to the data provided in blueprint
	if (Data != Room->GetRoomData())
	{
		LogError(FString::Printf(TEXT("RoomLevel's Data does not match RoomData's Level [Data \"%s\" | Level \"%s\"]"), *Room->GetRoomData()->GetName(), *GetName()));
	}

	if (URoom::OccludeDynamicActors())
	{
		// Create trigger box to track dynamic actors inside the room
		RoomTrigger = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("Room Trigger"));
		RoomTrigger->RegisterComponent();
		RoomTrigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		RoomTrigger->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel18); // Ignore line traces using any blueprint object type
		RoomTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		RoomTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		RoomTrigger->SetCanEverAffectNavigation(false);

		RoomTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoomLevel::OnTriggerBeginOverlap);
		RoomTrigger->OnComponentEndOverlap.AddDynamic(this, &ARoomLevel::OnTriggerEndOverlap);

		// Update trigger box to have the room's bounds
		FBoxCenterAndExtent LocalBounds = Room->GetLocalBounds();
		RoomTrigger->SetRelativeLocationAndRotation(LocalBounds.Center, FQuat::Identity);
		RoomTrigger->SetBoxExtent(LocalBounds.Extent, true);
	}

	SetActorsVisible(Room->IsVisible());

	bIsInit = true;
}

void ARoomLevel::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Update is called once per frame
void ARoomLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	// TODO: Place the debug draw in an editor module of the plugin
	if (URoom::DrawDebug() && IsValid(Data))
	{
		FTransform RoomTransform = (Room != nullptr) ? Room->GetTransform() : FTransform::Identity;
		UpdateBounds();

		// Pivot
		DrawDebugSphere(GetWorld(), DungeonTransform.TransformPosition(RoomTransform.GetLocation()), 100.0f, 4, FColor::Magenta);

		// Room bounds
		DrawDebugBox(GetWorld(), DungeonTransform.TransformPosition(Bounds.Center), Bounds.Extent, DungeonTransform.GetRotation(), IsPlayerInside() ? FColor::Green : FColor::Red);

		// Doors
		for (int i = 0; i < Data->GetNbDoor(); i++)
		{
			const bool isConnected = Room == nullptr || Room->IsConnected(i);
			const bool isDoorValid = Data->IsDoorValid(i);
			const FDoorDef& Door = Data->Doors[i];
			ADoor::DrawDebug(GetWorld(), Door.GetDoorSize(), Door.Position, Door.Direction, RoomTransform * DungeonTransform, true, isConnected, isDoorValid);
		}
	}
#endif
}

void ARoomLevel::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
		return;

	URoomVisibilityComponent* comp = Cast<URoomVisibilityComponent>(OtherActor->GetComponentByClass(URoomVisibilityComponent::StaticClass()));
	if (IsValid(comp) && !VisibilityComponents.Contains(comp))
	{
		VisibilityComponents.Add(comp);
		comp->SetVisible(this, IsVisible());
	}
}

void ARoomLevel::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor))
		return;

	URoomVisibilityComponent* comp = Cast<URoomVisibilityComponent>(OtherActor->GetComponentByClass(URoomVisibilityComponent::StaticClass()));
	if (IsValid(comp))
	{
		VisibilityComponents.Remove(comp);
		comp->ResetVisible(this);
	}
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
	ULevel* Level = GetLevel();
	if (IsValid(Level))
	{
		for (AActor* Actor : Level->Actors)
		{
			if (IsValid(Actor))
				Actor->SetActorHiddenInGame(!Visible);
		}
	}

	for (TWeakObjectPtr<URoomVisibilityComponent> Comp : VisibilityComponents)
	{
		if (!Comp.IsValid())
			VisibilityComponents.Remove(Comp);
		else
			Comp->SetVisible(this, Visible);
	}
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
}
#endif
