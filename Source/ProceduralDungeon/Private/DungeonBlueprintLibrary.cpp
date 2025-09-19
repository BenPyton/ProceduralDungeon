// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

const ARoomLevel* UDungeonBlueprintLibrary::GetLevelScript(const AActor* Target)
{
	if (!IsValid(Target))
		return nullptr;

	if (const ARoomLevel* SelfLevel = Cast<ARoomLevel>(Target))
	{
		return SelfLevel;
	}

	ULevel* Level = Target->GetLevel();
	if (!IsValid(Level))
		return nullptr;

	ARoomLevel* RoomLevel = Cast<ARoomLevel>(Level->GetLevelScriptActor());
	if (!IsValid(RoomLevel))
		return nullptr;

	return RoomLevel;
}

URoom* UDungeonBlueprintLibrary::GetOwningRoom(const AActor* Target)
{
	if (const ARoomLevel* SelfLevel = GetLevelScript(Target))
	{
		return SelfLevel->GetRoom();
	}
	return nullptr;
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

const URoomData* UDungeonBlueprintLibrary::GetLevelRoomData(const AActor* Target)
{
	if (const ARoomLevel* SelfLevel = GetLevelScript(Target))
	{
		return SelfLevel->GetRoomData();
	}
	return nullptr;
}

FDoorDef UDungeonBlueprintLibrary::DoorDef_GetOpposite(const FDoorDef& DoorDef)
{
	return (DoorDef) ? DoorDef.GetOpposite() : DoorDef;
}

// ===== Plugin Settings Accessors =====

FIntVector UDungeonBlueprintLibrary::IntVector_Next(const FIntVector& Vector, const EDoorDirection& Direction)
{
	return Vector + ToIntVector(Direction);
}

FIntVector UDungeonBlueprintLibrary::IntVector_Rotate(const FIntVector& Vector, const EDoorDirection& Direction)
{
	return Rotate(Vector, Direction);
}

FIntVector UDungeonBlueprintLibrary::Dungeon_TransformPosition(const FIntVector& LocalPos, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return Transform(LocalPos, Translation, Rotation);
}

FIntVector UDungeonBlueprintLibrary::Dungeon_InverseTransformPosition(const FIntVector& DungeonPos, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return InverseTransform(DungeonPos, Translation, Rotation);
}

FDoorDef UDungeonBlueprintLibrary::Dungeon_TransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return FDoorDef::Transform(DoorDef, Translation, Rotation);
}

FDoorDef UDungeonBlueprintLibrary::Dungeon_InverseTransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)
{
	return FDoorDef::InverseTransform(DoorDef, Translation, Rotation);
}

FIntVector UDungeonBlueprintLibrary::IntVector_Add(const FIntVector& A, const FIntVector& B)
{
	return A + B;
}

FIntVector UDungeonBlueprintLibrary::IntVector_Subtract(const FIntVector& A, const FIntVector& B)
{
	return A - B;
}

bool UDungeonBlueprintLibrary::IntVector_Equal(const FIntVector& A, const FIntVector& B)
{
	return A == B;
}

bool UDungeonBlueprintLibrary::IntVector_NotEqual(const FIntVector& A, const FIntVector& B)
{
	return A != B;
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
