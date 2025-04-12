/*
 * Copyright (c) 2023 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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

FDoorDef UDungeonBlueprintLibrary::DoorDef_GetOpposite(const FDoorDef& DoorDef)
{
	return (DoorDef) ? DoorDef.GetOpposite() : DoorDef;
}

// ===== Plugin Settings Accessors =====

FIntVector UDungeonBlueprintLibrary::Dungeon_TransformPosition(const FIntVector& LocalPos, const FIntVector& Offset, const EDoorDirection& Rotation)
{
	return Rotate(LocalPos, Rotation) + Offset;
}

FIntVector UDungeonBlueprintLibrary::Dungeon_InverseTransformPosition(const FIntVector& DungeonPos, const FIntVector& Offset, const EDoorDirection& Rotation)
{
	return Rotate(DungeonPos - Offset, -Rotation);
}

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
