// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "DoorType.h"
#include "ProceduralDungeonUtils.h"

UDoorType::UDoorType()
	: UDataAsset()
{
	Size = Dungeon::DefaultDoorSize();
	Offset = Dungeon::DoorOffset();
#if WITH_EDITORONLY_DATA
	Color = FColor::Blue;
	Description = FText::FromString(TEXT("No Description"));
#endif
}

FVector UDoorType::GetSize(const UDoorType* DoorType)
{
	return IsValid(DoorType) ? DoorType->Size : Dungeon::DefaultDoorSize();
}

float UDoorType::GetOffset(const UDoorType* DoorType)
{
	return IsValid(DoorType) ? DoorType->Offset : Dungeon::DoorOffset();
}

FColor UDoorType::GetColor(const UDoorType* DoorType)
{
	return IsValid(DoorType) ? DoorType->Color : Dungeon::DefaultDoorColor();
}
