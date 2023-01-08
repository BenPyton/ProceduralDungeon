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

	bLocked = !bAlwaysUnlocked && 
		   ((RoomA == nullptr || (RoomA->GetLevelScript() != nullptr && RoomA->GetLevelScript()->IsLocked))
		||  (RoomB == nullptr || (RoomB->GetLevelScript() != nullptr && RoomB->GetLevelScript()->IsLocked)));

	SetActorHiddenInGame( !bAlwaysVisible &&
			(RoomA == nullptr || (RoomA->GetLevelScript() != nullptr && RoomA->GetLevelScript()->IsHidden)) 
		&&	(RoomB == nullptr || (RoomB->GetLevelScript() != nullptr && RoomB->GetLevelScript()->IsHidden)));

	if (bLocked != bPrevLocked)
	{
		if (bLocked)
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

	bPrevLocked = bLocked;

#if WITH_EDITOR
	// TODO: Place it in an editor module of the plugin
	DrawDebug(GetWorld());
#endif
}

void ADoor::OpenDoor()
{
	if (!bIsOpen && !bLocked)
	{
		bIsOpen = true;
		OnDoorOpen();
		OnDoorOpen_BP();
	}
}

void ADoor::CloseDoor()
{
	if (bIsOpen)
	{
		bIsOpen = false;
		OnDoorClose();
		OnDoorClose_BP();
	}
}

void ADoor::SetConnectingRooms(URoom* _RoomA, URoom* _RoomB)
{
	RoomA = _RoomA;
	RoomB = _RoomB;
}

void ADoor::DrawDebug(UWorld* World, FIntVector DoorCell, EDoorDirection DoorRot, FTransform Transform, bool includeOffset, bool isConnected)
{
	if (URoom::DrawDebug())
	{
		FVector DoorSize = URoom::DoorSize();
		FQuat DoorRotation = Transform.GetRotation() * URoom::GetRotation(DoorRot == EDoorDirection::NbDirection ? EDoorDirection::North : DoorRot);
		FVector DoorPosition = Transform.TransformPosition(URoom::GetRealDoorPosition(DoorCell, DoorRot, includeOffset) + FVector(0, 0, DoorSize.Z * 0.5f));

		// Door frame
		DrawDebugBox(World, DoorPosition, DoorSize * 0.5f, DoorRotation, FColor::Blue);

		if (isConnected)
		{
			// Arrow (there is a room on the other side OR in the editor preview)
			DrawDebugDirectionalArrow(World, DoorPosition, DoorPosition + DoorRotation * FVector(300, 0, 0), 300, FColor::Blue);
		}
		else
		{
			// Cross (there is no room on the other side)
			FVector HalfSize = DoorRotation * FVector(0, DoorSize.Y, DoorSize.Z) * 0.5f;
			FVector HalfSizeConjugate = DoorRotation * FVector(0, DoorSize.Y, -DoorSize.Z) * 0.5f;
			DrawDebugLine(World, DoorPosition - HalfSize, DoorPosition + HalfSize, FColor::Blue);
			DrawDebugLine(World, DoorPosition - HalfSizeConjugate, DoorPosition + HalfSizeConjugate, FColor::Blue);
		}
	}
}

