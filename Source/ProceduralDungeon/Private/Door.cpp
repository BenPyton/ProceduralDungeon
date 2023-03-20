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

#include "Door.h"
#include "Room.h"
#include "RoomLevel.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "DungeonGenerator.h"

// Sets default values
ADoor::ADoor()
	: RoomA(nullptr)
	, RoomB(nullptr)
	, IndexRoomA(-1)
	, IndexRoomB(-1)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true; // prevent the doors from despawning on clients when server's player is too far

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, IndexRoomA);
	DOREPLIFETIME(ADoor, IndexRoomB);
	DOREPLIFETIME(ADoor, bShouldBeLocked);
	DOREPLIFETIME(ADoor, bShouldBeOpen);
}

void ADoor::OnRep_IndexRoomA()
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Blue, FString::Printf(TEXT("[Door %s] Replicated index room A: %d"), *GetName(), IndexRoomA));
	//UE_LOG(LogTemp, Log, TEXT("[Door %s] Replicated index room A: %d"), *GetName(), IndexRoomA);
	if (IndexRoomA >= 0)
	{
		ADungeonGenerator* Generator = Cast<ADungeonGenerator>(GetOwner());
		if (IsValid(Generator))
		{
			RoomA = Generator->GetRoomByIndex(IndexRoomA);
			//UE_LOG(LogTemp, Log, TEXT("[Door %s] Replicated Room A: %s"), *GetName(), IsValid(RoomA) ? *RoomA->GetName() : TEXT("NULL"));
		}
	}
}

void ADoor::OnRep_IndexRoomB()
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Blue, FString::Printf(TEXT("[Door %s] Replicated index room B: %d"), *GetName(), IndexRoomB));
	//UE_LOG(LogTemp, Log, TEXT("[Door %s] Replicated index room B: %d"), *GetName(), IndexRoomB);
	if (IndexRoomB >= 0)
	{
		ADungeonGenerator* Generator = Cast<ADungeonGenerator>(GetOwner());
		if (IsValid(Generator))
		{
			RoomB = Generator->GetRoomByIndex(IndexRoomB);
			//UE_LOG(LogTemp, Log, TEXT("[Door %s] Replicated Room B: %s"), *GetName(), IsValid(RoomB) ? *RoomB->GetName() : TEXT("NULL"));
		}
	}
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Blue, FString::Printf(TEXT("Door owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("NULL")));
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update door visibility
	SetActorHiddenInGame( !bAlwaysVisible
		&& (!IsValid(RoomA) || (!RoomA->IsVisible())) 
		&& (!IsValid(RoomB) || (!RoomB->IsVisible())));

	// Update door's lock state
	const bool bPrevLocked = bLocked;
	bLocked = !bAlwaysUnlocked && (bShouldBeLocked ||
			  ((!IsValid(RoomA) || RoomA->IsLocked())
			|| (!IsValid(RoomB) || RoomB->IsLocked())));

	if (bLocked != bPrevLocked)
	{
		if (bLocked)
		{
			OnDoorLock();
			OnDoorLock_BP();
		}
		else
		{
			OnDoorUnlock();
			OnDoorUnlock_BP();
		}
	}

	// Update door's open state
	const bool bPrevIsOpen = bIsOpen;
	bIsOpen = bShouldBeOpen && !bLocked;
	if (bIsOpen != bPrevIsOpen)
	{
		if (bIsOpen)
		{
			OnDoorOpen();
			OnDoorOpen_BP();
		}
		else
		{
			OnDoorClose();
			OnDoorClose_BP();
		}
	}

#if WITH_EDITOR
	// TODO: Place it in an editor module of the plugin
	if (GetWorld()->WorldType == EWorldType::EditorPreview)
		DrawDebug(GetWorld());
#endif
}

void ADoor::SetConnectingRooms(URoom* _RoomA, URoom* _RoomB)
{
	RoomA = _RoomA;
	RoomB = _RoomB;
	IndexRoomA = IsValid(_RoomA) ? _RoomA->GetRoomID() : -1;
	IndexRoomB = IsValid(_RoomB) ? _RoomB->GetRoomID() : -1;
	//UE_LOG(LogTemp, Log, TEXT("[Door %s] Set index room A: %d | B: %d"), *GetName(), IndexRoomA, IndexRoomB);
}

void ADoor::DrawDebug(UWorld* World, FIntVector DoorCell, EDoorDirection DoorRot, FTransform Transform, bool includeOffset, bool isConnected, bool isValid)
{
	if (URoom::DrawDebug())
	{
		FVector DoorSize = URoom::DoorSize();
		FQuat DoorRotation = Transform.GetRotation() * URoom::GetRotation(DoorRot == EDoorDirection::NbDirection ? EDoorDirection::North : DoorRot);
		FVector DoorPosition = Transform.TransformPosition(URoom::GetRealDoorPosition(DoorCell, DoorRot, includeOffset) + FVector(0, 0, DoorSize.Z * 0.5f));

		const FColor& Color = (isValid) ? FColor::Blue : FColor::Orange;

		// Door frame
		DrawDebugBox(World, DoorPosition, DoorSize * 0.5f, DoorRotation, Color);

		if (isConnected)
		{
			// Arrow (there is a room on the other side OR in the editor preview)
			DrawDebugDirectionalArrow(World, DoorPosition, DoorPosition + DoorRotation * FVector(300, 0, 0), 300, Color);
		}
		else
		{
			// Cross (there is no room on the other side)
			FVector HalfSize = DoorRotation * FVector(0, DoorSize.Y, DoorSize.Z) * 0.5f;
			FVector HalfSizeConjugate = DoorRotation * FVector(0, DoorSize.Y, -DoorSize.Z) * 0.5f;
			DrawDebugLine(World, DoorPosition - HalfSize, DoorPosition + HalfSize, Color);
			DrawDebugLine(World, DoorPosition - HalfSizeConjugate, DoorPosition + HalfSizeConjugate, Color);
		}
	}
}
