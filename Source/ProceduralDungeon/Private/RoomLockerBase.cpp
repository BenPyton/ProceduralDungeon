/*
 * MIT License
 *
 * Copyright (c) 2019-2021, 2023 Benoit Pelletier
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

#include "RoomLockerBase.h"
#include "RoomLevel.h"
#include "Room.h"
#include "RoomData.h"
#include "GameFramework/GameState.h"
#include "Engine/World.h"

void ADEPRECATED_RoomLockerBase::SetLocked(bool Locked, bool Self, TSubclassOf<URoomData> RoomType)
{
	ARoomLevel* Script = GetRoomLevel();
	if (nullptr != Script)
	{
		if (Self)
		{
			Script->Lock(Locked);
		}

		URoom* Room = Script->Room;
		if (nullptr != Room && nullptr != RoomType)
		{
			for (int i = 0; i < Room->GetConnectionCount(); i++)
			{
				if (nullptr != Room->GetConnection(i))
				{
					if (RoomType == Room->GetConnection(i)->GetRoomData()->GetClass())
					{
						Room->GetConnection(i)->Lock(Locked);
					}
				}
			}
		}
	}
}

ARoomLevel * ADEPRECATED_RoomLockerBase::GetRoomLevel()
{
	return Cast<ARoomLevel>(GetLevel()->GetLevelScriptActor());
}

