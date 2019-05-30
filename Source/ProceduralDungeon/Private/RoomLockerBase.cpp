// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomLockerBase.h"
#include "RoomLevel.h"
#include "Room.h"
#include "RoomData.h"
#include "GameFramework/GameState.h"
#include "Engine/World.h"

void ARoomLockerBase::SetLocked(bool Locked, bool Self, TSubclassOf<URoomData> RoomType)
{
	ARoomLevel* Script = GetRoomLevel();
	if (nullptr != Script)
	{
		if (Self)
		{
			Script->IsLocked = Locked;
		}

		URoom* Room = Script->Room;
		if (nullptr != Room && nullptr != RoomType)
		{
			for (int i = 0; i < Room->GetConnectionCount(); i++)
			{
				if (nullptr != Room->GetConnection(i) && nullptr != Room->GetConnection(i)->GetLevelScript())
				{
					if (RoomType == Room->GetConnection(i)->GetRoomDataClass())
					{
						Room->GetConnection(i)->GetLevelScript()->IsLocked = Locked;
					}
				}
			}
		}
	}
}

ARoomLevel * ARoomLockerBase::GetRoomLevel()
{
	return Cast<ARoomLevel>(GetLevel()->GetLevelScriptActor());
}

