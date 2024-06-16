/*
* MIT License
*
* Copyright (c) 2024 Benoit Pelletier
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

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "RoomData.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRoomDataTests, "ProceduralDungeon.Types.RoomData", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

// Utility to create room data
#define CREATE_ROOM_DATA(Data) \
	TStrongObjectPtr<URoomData> Data(NewObject<URoomData>(GetTransientPackage(), #Data)); \
	Data->Doors.Empty();

bool FRoomDataTests::RunTest(const FString& Parameters)
{
	// Test IsRoomInBounds
	{
		// Creating this room data (X is the room origin, v is the door), thus we could test rotated bounds:
		//		1 +---+---+---+
		//		  |   | X |   |
		//		0 +---+---+---+
		//		  |   |   |   |
		//	   -1 +---+---+---+
		//		  |   |   |   |
		//	   -2 +---+-v-+---+
		//		 -1   0   1   2
		CREATE_ROOM_DATA(RoomData);
		RoomData->FirstPoint = FIntVector(-2, -1, -1);
		RoomData->SecondPoint = FIntVector(1, 2, 2);

		FDoorDef Door;
		Door.Position = FIntVector(-2, 0, 0);
		Door.Direction = EDoorDirection::South;
		RoomData->Doors.Add(Door);

		// If we want to limit the dungeon cells from -2 to 2,
		// we need to create a box from -2 to 3 (see below).
		//		+---+---+---+---+---+
		//		|-2 |-1 | 0 | 1 | 2 |
		//		+---+---+---+---+---+
		//	   -2  -1   0   1   2   3
		FBoxMinAndMax DungeonBounds({-1000, -2, -1000}, {1000, 3, 1000});

		FBoxMinAndMax RoomBoundsAtDoorLocation = RoomData->GetIntBounds() - Door.Position;

		// Rotated to South
		{
			FBoxMinAndMax RotatedRoomBounds = Rotate(RoomBoundsAtDoorLocation, EDoorDirection::North);
			TestEqual(TEXT("[S] Rotated Room Bounds: ((0, -1, -1), (3, 2, 2))"), RotatedRoomBounds, FBoxMinAndMax({0, -1, -1}, {3, 2, 2}));

			Door.Direction = EDoorDirection::South;
			Door.Position = {0, 0, 0};
			TestTrue(TEXT("[S] Inside with no coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Positive Y

			Door.Position = {0, 1, 0};
			TestTrue(TEXT("[S] Inside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 2, 0};
			TestFalse(TEXT("[S] Intersecting in Y+"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 4, 0};
			TestFalse(TEXT("[S] Outside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Negative Y

			Door.Position = {0, -1, 0};
			TestTrue(TEXT("[S] Inside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -2, 0};
			TestFalse(TEXT("[S] Intersecting in Y-"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -4, 0};
			TestFalse(TEXT("[S] Outside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));
		}

		// Rotated to East
		{
			FBoxMinAndMax RotatedRoomBounds = Rotate(RoomBoundsAtDoorLocation, EDoorDirection::West);
			TestEqual(TEXT("[E] Rotated Room Bounds: ((-1, -2, -1), (2, 1, 2))"), RotatedRoomBounds, FBoxMinAndMax({-1, -2, -1}, {2, 1, 2}));

			Door.Direction = EDoorDirection::East;
			Door.Position = {0, 1, 0};
			TestTrue(TEXT("[E] Inside with no coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Positive Y

			Door.Position = {0, 2, 0};
			TestTrue(TEXT("[E] Inside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 3, 0};
			TestFalse(TEXT("[E] Intersecting in Y+"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 5, 0};
			TestFalse(TEXT("[E] Outside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Negative Y

			Door.Position = {0, 0, 0};
			TestTrue(TEXT("[E] Inside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -1, 0};
			TestFalse(TEXT("[E] Intersecting in Y-"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -3, 0};
			TestFalse(TEXT("[E] Outside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));
		}

		// Rotated to West
		{
			FBoxMinAndMax RotatedRoomBounds = Rotate(RoomBoundsAtDoorLocation, EDoorDirection::East);
			TestEqual(TEXT("[W] Rotated Room Bounds: ((-1, 0, -1), (2, 3, 2))"), RotatedRoomBounds, FBoxMinAndMax({-1, 0, -1}, {2, 3, 2}));

			Door.Direction = EDoorDirection::West;
			Door.Position = {0, -1, 0};
			TestTrue(TEXT("[W] Inside with no coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Positive Y

			Door.Position = {0, 0, 0};
			TestTrue(TEXT("[W] Inside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 1, 0};
			TestFalse(TEXT("[W] Intersecting in Y+"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 3, 0};
			TestFalse(TEXT("[W] Outside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Negative Y

			Door.Position = {0, -2, 0};
			TestTrue(TEXT("[W] Inside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -3, 0};
			TestFalse(TEXT("[W] Intersecting in Y-"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -5, 0};
			TestFalse(TEXT("[W] Outside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));
		}

		// Rotated to North
		{
			FBoxMinAndMax RotatedRoomBounds = Rotate(RoomBoundsAtDoorLocation, EDoorDirection::South);
			TestEqual(TEXT("[N] Rotated Room Bounds: ((-2, -1, -1), (1, 2, 2))"), RotatedRoomBounds, FBoxMinAndMax({-2, -1, -1}, {1, 2, 2}));

			Door.Direction = EDoorDirection::North;
			Door.Position = {0, 0, 0};
			TestTrue(TEXT("[N] Inside with no coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Positive Y

			Door.Position = {0, 1, 0};
			TestTrue(TEXT("[N] Inside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 2, 0};
			TestFalse(TEXT("[N] Intersecting in Y+"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, 4, 0};
			TestFalse(TEXT("[N] Outside with Y+ as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			// Negative Y

			Door.Position = {0, -1, 0};
			TestTrue(TEXT("[N] Inside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -2, 0};
			TestFalse(TEXT("[N] Intersecting in Y-"), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));

			Door.Position = {0, -4, 0};
			TestFalse(TEXT("[N] Outside with Y- as coincident face."), RoomData->IsRoomInBounds(DungeonBounds, 0, Door));
		}
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
