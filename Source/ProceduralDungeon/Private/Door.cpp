// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Room.h"
#include "RoomLevel.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	locked = !AlwaysUnlocked && 
		   ((RoomA == nullptr || (RoomA->GetLevelScript() != nullptr && RoomA->GetLevelScript()->IsLocked))
		||  (RoomB == nullptr || (RoomB->GetLevelScript() != nullptr && RoomB->GetLevelScript()->IsLocked)));

	SetActorHiddenInGame( !AlwaysVisible &&
			(RoomA == nullptr || (RoomA->GetLevelScript() != nullptr && RoomA->GetLevelScript()->IsHidden)) 
		&&	(RoomB == nullptr || (RoomB->GetLevelScript() != nullptr && RoomB->GetLevelScript()->IsHidden)));

	if (locked != prevLocked)
	{
		if (locked)
		{
			CloseDoor();
			OnDoorLock();
			OnDoorLock_BP();
		}
		else
		{
			OnDoorUnlock();
			OnDoorUnlock_BP();
		}
	}

	prevLocked = locked;

#if WITH_EDITOR
	DrawDebug(GetWorld());
#endif
}

void ADoor::OpenDoor()
{
	if (!isOpen && !locked)
	{
		isOpen = true;
		OnDoorOpen();
		OnDoorOpen_BP();
	}
}

void ADoor::CloseDoor()
{
	if (isOpen)
	{
		isOpen = false;
		OnDoorClose();
		OnDoorClose_BP();
	}
}

void ADoor::SetConnectingRooms(URoom * _roomA, URoom * _roomB)
{
	RoomA = _roomA;
	RoomB = _roomB;
}

void ADoor::DrawDebug(UWorld* World, FIntVector DoorCell, EDoorDirection DoorRot, FTransform Transform)
{
	if (URoom::DrawDebug())
	{
		FVector DoorSize = URoom::DoorSize();
		FIntVector rot = URoom::GetDirection(DoorRot == EDoorDirection::NbDirection ? EDoorDirection::North : DoorRot);
		FVector pos = URoom::GetRealDoorPosition(DoorCell, DoorRot) + FVector(0, 0, DoorSize.Z * 0.5f);
		pos = Transform.TransformPosition(pos);

		// Arrow
		DrawDebugDirectionalArrow(World, pos, pos + Transform.GetRotation() * FVector(rot) * 300, 300, FColor::Blue);

		// Door frame
		FIntVector scale = URoom::Rotate(FIntVector(DoorSize * 0.5f), DoorRot);
		DrawDebugBox(World, pos, FVector(scale), Transform.GetRotation(), FColor::Blue);
	}
}

