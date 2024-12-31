/*
 * MIT License
 *
 * Copyright (c) 2025 Benoit Pelletier
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

#include "Interfaces/RoomActorGuid.h"
#include "ProceduralDungeonLog.h"

FGuid IRoomActorGuid::GetActorGuid(AActor* Actor)
{
	if (!IsValid(Actor))
		return FGuid();

	FGuid ActorGuid = (Actor->Implements<URoomActorGuid>()) ? IRoomActorGuid::Execute_GetGuid(Actor) : FGuid();
	if (!ActorGuid.IsValid())
	{
		const auto Components = Actor->GetComponentsByInterface(URoomActorGuid::StaticClass());
		for (const auto* Component : Components)
		{
			if (!ActorGuid.IsValid())
			{
				ActorGuid = IRoomActorGuid::Execute_GetGuid(Component);
			}
			else
			{
				DungeonLog_WarningSilent("Multiple components have a RoomActorGuid implemented. Remove the unnecessary ones to prevent any confusion!");
			}
		}
	}

	return ActorGuid;
}
