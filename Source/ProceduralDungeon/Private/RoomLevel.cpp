/*
 * MIT License
 *
 * Copyright (c) 2019-2021 Benoit Pelletier
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
#include "CoreUObject.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Pawn.h"
#include "ProceduralDungeonTypes.h"
#include "Room.h"
#include "RoomData.h"
#include "Door.h"


uint32 ARoomLevel::Count = 0;

// Use this for initialization
void ARoomLevel::Init(URoom* _Room)
{
	Id = Count;
	Count++;

	IsInit = false;
	Room = _Room;
	PendingInit = true;
}

ARoomLevel::ARoomLevel(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	IsInit = false;
	PendingInit = false;
	Room = nullptr;
}

void ARoomLevel::BeginPlay()
{
	Super::BeginPlay();
}

void ARoomLevel::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (AActor* Actor : ActorsInLevel)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

// Update is called once per frame
void ARoomLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInit)
	{
		if (PendingInit && Room != nullptr)
		{
			Transform.SetLocation(FVector(Room->Position) * URoom::Unit());
			Transform.SetRotation(FRotator(0.0f, -90.0f * (int8)Room->Direction, 0.0f).Quaternion());

			FIntVector forward = URoom::GetDirection(Room->Direction);
			FIntVector right = URoom::GetDirection(URoom::Add(Room->Direction, EDoorDirection::East));

			// Create triggerBox for occlusion culling
			Center = 0.5f * (URoom::Unit() * FVector(Room->Position + Room->RoomToWorld(Room->Values->Size) - forward - right));
			HalfExtents = 0.5f * (URoom::Unit() * FVector(Room->RoomToWorld(Room->Values->Size) - Room->Position));
			HalfExtents = FVector(FMath::Abs(HalfExtents.X), FMath::Abs(HalfExtents.Y), FMath::Abs(HalfExtents.Z));

			// Register All Actors in the level
			for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				ULevel *Level = ActorItr->GetLevel();
				if (Level->GetOuter() == GetLevel()->GetOuter())
				{
					ActorsInLevel.Add(*ActorItr);
				}
			}

			PendingInit = false;
			IsInit = true;
		}
	}
	else
	{
		Display();
	}

	if (Data == nullptr)
		return;

	URoomData* Values = Data.GetDefaultObject();

	FIntVector forward = URoom::GetDirection(EDoorDirection::North);
	FIntVector right = URoom::GetDirection(URoom::Add(EDoorDirection::North, EDoorDirection::East));

	Center = 0.5f * (URoom::Unit() * FVector(Values->Size - forward - right));
	HalfExtents = 0.5f * (URoom::Unit() * FVector(Values->Size));
	HalfExtents = FVector(FMath::Abs(HalfExtents.X), FMath::Abs(HalfExtents.Y), FMath::Abs(HalfExtents.Z));

	Center = Transform.TransformPosition(Center);

#if WITH_EDITOR
	if (URoom::DrawDebug())
	{
		// Pivot
		DrawDebugSphere(GetWorld(), Transform.GetLocation(), 100.0f, 4, FColor::Magenta);

		// Room bounds
		DrawDebugBox(GetWorld(), Center, HalfExtents, Transform.GetRotation(), FColor::Red);

		FVector DoorSize = URoom::DoorSize();

		// Doors
		for (int i = 0; i < Values->Doors.Num(); i++)
		{
			ADoor::DrawDebug(GetWorld(), Values->Doors[i].Position, Values->Doors[i].Direction, Transform);
		}
	}
#endif
}

bool ARoomLevel::IsPlayerInside()
{
	bool inside = false;

	FCollisionShape box = FCollisionShape::MakeBox(HalfExtents);

	APawn* player = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawnOrSpectator();
	TArray<FOverlapResult> overlappedActors;

	if (GetWorld()->OverlapMultiByObjectType(
		overlappedActors,
		Center,
		Transform.GetRotation(),
		FCollisionObjectQueryParams::AllDynamicObjects,
		box))
	{
		for (FOverlapResult result : overlappedActors)
		{
			if (player == result.GetActor())
			{
				inside = true;
			}
		}
	}
	return inside;
}

void ARoomLevel::Display()
{
	if (!IsPendingKill() && Room != nullptr && URoom::OcclusionCulling())
	{
		PlayerInside = IsPlayerInside();
		IsHidden = !PlayerInside;
		for (int i = 0; i < Room->GetConnectionCount(); i++)
		{
			if (Room->GetConnection(i) != nullptr
				&& IsValid(Room->GetConnection(i)->GetLevelScript())
				&& Room->GetConnection(i)->GetLevelScript()->PlayerInside)
			{
				IsHidden = false;
			}
		}

		// force IsHidden to false if AlwaysVisible is true
		IsHidden &= !AlwaysVisible;

		for (AActor* Actor : ActorsInLevel)
		{
			if (IsValid(Actor))
			{
				Actor->SetActorHiddenInGame(IsHidden);
			}
		}
	}
}
