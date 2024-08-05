/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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

#include "DungeonBlueprintLibrary.h"
#include "Door.h"
#include "DoorType.h"
#include "ProceduralDungeonUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "Roomlevel.h"
#include "Room.h"
#include "RoomCustomData.h"
#include "Engine/Engine.h" // GEngine

bool UDungeonBlueprintLibrary::IsDoorOfType(const TSubclassOf<ADoor> DoorClass, const UDoorType* DoorType)
{
	ADoor* Door = DoorClass.GetDefaultObject();
	return Door && (Door->GetDoorType() == DoorType);
}

bool UDungeonBlueprintLibrary::CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B)
{
	return A == B;
}

URoom* UDungeonBlueprintLibrary::GetOwningRoom(const AActor* Target)
{
	if (!IsValid(Target))
		return nullptr;

	if (const ARoomLevel* SelfLevel = Cast<ARoomLevel>(Target))
	{
		return SelfLevel->Room;
	}

	ULevel* Level = Target->GetLevel();
	if (!IsValid(Level))
		return nullptr;

	ARoomLevel* RoomLevel = Cast<ARoomLevel>(Level->GetLevelScriptActor());
	if (!IsValid(RoomLevel))
		return nullptr;

	return RoomLevel->GetRoom();
}

bool UDungeonBlueprintLibrary::GetOwningRoomCustomData(const AActor* Target, TSubclassOf<URoomCustomData> CustomDataClass, URoomCustomData*& CustomData)
{
	CustomData = nullptr;
	URoom* OwningRoom = GetOwningRoom(Target);
	if (!IsValid(OwningRoom))
		return false;

	OwningRoom->GetCustomData(CustomDataClass, CustomData);
	return IsValid(CustomData);
}

// ===== Plugin Settings Accessors =====

FVector UDungeonBlueprintLibrary::Settings_RoomUnit()
{
	return Dungeon::RoomUnit();
}

FVector UDungeonBlueprintLibrary::Settings_DefaultDoorSize()
{
	return Dungeon::DefaultDoorSize();
}

float UDungeonBlueprintLibrary::Settings_DoorOffset()
{
	return Dungeon::DoorOffset();
}

bool UDungeonBlueprintLibrary::Settings_OcclusionCulling()
{
	return Dungeon::OcclusionCulling();
}

void UDungeonBlueprintLibrary::Settings_SetOcclusionCulling(bool Enable)
{
	Dungeon::EnableOcclusionCulling(Enable);
}

int32 UDungeonBlueprintLibrary::Settings_OcclusionDistance()
{
	return Dungeon::OcclusionDistance();
}

void UDungeonBlueprintLibrary::Settings_SetOcclusionDistance(int32 Distance)
{
	Dungeon::SetOcclusionDistance(Distance);
}

bool UDungeonBlueprintLibrary::Settings_OccludeDynamicActors()
{
	return Dungeon::OccludeDynamicActors();
}

// ===== Gameplay Utility Functions =====

void UDungeonBlueprintLibrary::Spectate(APlayerController* Controller, bool DestroyPawn)
{
	if (!Controller)
		return;

	if (!Controller->HasAuthority())
		return;

	APawn* PreviousPawn = Controller->GetPawn();

	Controller->PlayerState->SetIsSpectator(true);
	Controller->ChangeState(NAME_Spectating);
	Controller->bPlayerIsWaiting = true;
	Controller->ClientGotoState(NAME_Spectating);

	if (DestroyPawn && IsValid(PreviousPawn))
	{
		PreviousPawn->Destroy();
	}
}
