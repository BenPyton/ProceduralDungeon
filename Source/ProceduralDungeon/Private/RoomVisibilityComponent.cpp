// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "RoomVisibilityComponent.h"
#include "ProceduralDungeonUtils.h"
#include "ProceduralDungeonLog.h"
#include "RoomLevel.h"

URoomVisibilityComponent::URoomVisibilityComponent()
	: Super()
{
}

void URoomVisibilityComponent::OnRoomEnter_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_Debug("[Visibility] '%s' Enters Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	RegisterVisibilityDelegate(RoomLevel, true);
}

void URoomVisibilityComponent::OnRoomExit_Implementation(ARoomLevel* RoomLevel)
{
	DungeonLog_Debug("[Visibility] '%s' Exits Room: %s", *GetNameSafe(GetOwner()), *GetNameSafe(RoomLevel));
	RegisterVisibilityDelegate(RoomLevel, false);
}
