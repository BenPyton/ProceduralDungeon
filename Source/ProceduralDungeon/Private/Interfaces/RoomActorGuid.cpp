// Copyright Benoit Pelletier 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#include "Interfaces/RoomActorGuid.h"
#include "GameFramework/Actor.h"
#include "ProceduralDungeonLog.h"

UObject* IRoomActorGuid::GetImplementer(AActor* Actor)
{
	if (!IsValid(Actor))
		return nullptr;

	if (Actor->Implements<URoomActorGuid>())
		return Actor;

	const auto Components = Actor->GetComponentsByInterface(URoomActorGuid::StaticClass());
	if (Components.Num() <= 0)
		return nullptr;

	if (Components.Num() > 1)
	{
		DungeonLog_WarningSilent("Multiple components have a RoomActorGuid interface. Remove the unnecessary ones to prevent any confusion!");
	}

	return Components[0];
}
