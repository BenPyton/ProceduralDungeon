/*
* MIT License
*
* Copyright (c) 2023-2024 Benoit Pelletier
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
#include "DungeonGraph.h"
#include "Room.h"
#include "RoomData.h"
#include "UObject/StrongObjectPtr.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonGraphTest, "ProceduralDungeon.Types.DungeonGraph", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)

#define INIT_TEST(Graph) \
	TStrongObjectPtr<UDungeonGraph> Graph(NewObject<UDungeonGraph>(GetTransientPackage(), #Graph)); \

#define CLEAN_TEST() \
	Graph->Clear();

// Utility to create and initialize a room
#define CREATE_ROOM(Name, RoomDataPtr) \
	URoom* Name = NewObject<URoom>(); \
	Name->Init(RoomDataPtr.Get(), nullptr, Graph->Count()); \
	Graph->AddRoom(Name);

// Utility to create room data
#define CREATE_ROOM_DATA(Data) \
	TStrongObjectPtr<URoomData> Data(NewObject<URoomData>(GetTransientPackage(), #Data)); \
	Data->Doors.Empty();

// Utility to create a non-empty path
#define DUMMY_PATH(Path) \
	Path.Empty(); \
	Path.Add(nullptr);

bool FDungeonGraphTest::RunTest(const FString& Parameters)
{
	{
		// Creating data assets
		CREATE_ROOM_DATA(DA_A);
		CREATE_ROOM_DATA(DA_B);
		CREATE_ROOM_DATA(DA_C);

		DA_A->Doors.Add({{0,0,0}, EDoorDirection::South});

		DA_B->Doors.Add({{0,0,0}, EDoorDirection::East});
		DA_B->Doors.Add({{0,0,0}, EDoorDirection::West});

		DA_C->Doors.Add({{0,0,0}, EDoorDirection::North});
		DA_C->Doors.Add({{0,0,0}, EDoorDirection::South});
		DA_C->Doors.Add({{0,0,0}, EDoorDirection::East});
		DA_C->Doors.Add({{0,0,0}, EDoorDirection::West});

		// Test pathfind
		{
			INIT_TEST(Graph);

			// (Rooms are numbered from left to right and top to bottom starting at 0)
			// A--C-C-!C-C-B
			//    | |    |
			// A-!B B--B-C

			// first line
			CREATE_ROOM(Room0, DA_A);
			CREATE_ROOM(Room1, DA_C);
			CREATE_ROOM(Room2, DA_C);
			CREATE_ROOM(Room3, DA_C);
			CREATE_ROOM(Room4, DA_C);
			CREATE_ROOM(Room5, DA_B);

			// second line
			CREATE_ROOM(Room6, DA_A);
			CREATE_ROOM(Room7, DA_B);
			CREATE_ROOM(Room8, DA_B);
			CREATE_ROOM(Room9, DA_B);
			CREATE_ROOM(Room10, DA_C);

			Room3->Lock(true);
			Room7->Lock(true);

			// first line
			URoom::Connect(*Room0, 0, *Room1, 1);
			URoom::Connect(*Room1, 0, *Room2, 1);
			URoom::Connect(*Room2, 0, *Room3, 1);
			URoom::Connect(*Room3, 0, *Room4, 1);
			URoom::Connect(*Room4, 0, *Room5, 1);

			// second line
			URoom::Connect(*Room6, 0, *Room7, 0);
			URoom::Connect(*Room8, 0, *Room9, 1);
			URoom::Connect(*Room9, 0, *Room10, 0);

			// transversal
			URoom::Connect(*Room1, 2, *Room7, 1);
			URoom::Connect(*Room2, 2, *Room8, 1);
			URoom::Connect(*Room4, 2, *Room10, 1);

			// Used to test path output for some scenarios
			TArray<const URoom*> Path;

			// Check graph state
			TestEqual(TEXT("Graph should have 11 rooms"), 11, Graph->Count());
			TestEqual(TEXT("Graph should have 2 rooms from data DA_A"), 2, Graph->CountRoomData(DA_A.Get()));
			TestEqual(TEXT("Graph should have 4 rooms from data DA_B"), 4, Graph->CountRoomData(DA_B.Get()));
			TestEqual(TEXT("Graph should have 5 rooms from data DA_C"), 5, Graph->CountRoomData(DA_C.Get()));

			// Check path find with null rooms
			DUMMY_PATH(Path);
			TestFalse(TEXT("Path should not be found when both rooms are null"), UDungeonGraph::FindPath(nullptr, nullptr, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);
			DUMMY_PATH(Path);
			TestFalse(TEXT("Path should not be found when first room is null"), UDungeonGraph::FindPath(nullptr, Room1, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);
			DUMMY_PATH(Path);
			TestFalse(TEXT("Path should not be found when second room is null"), UDungeonGraph::FindPath(Room0, nullptr, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);

			// Check path found with same room
			TestTrue(TEXT("Path should be found when both rooms are same"), UDungeonGraph::FindPath(Room0, Room0, &Path));
			TestEqual(TEXT("Path should have 1 room"), Path.Num(), 1);
			TestTrue(TEXT("Path should have Room0"), Path[0] == Room0);
			TestTrue(TEXT("Path should be found when both rooms are same (even if locked)"), UDungeonGraph::FindPath(Room3, Room3, &Path));
			TestEqual(TEXT("Path should have 1 room"), Path.Num(), 1);
			TestTrue(TEXT("Path should have Room3"), Path[0] == Room3);

			// Check no path when one or both rooms are locked
			DUMMY_PATH(Path);
			TestFalse(TEXT("No path should be found between Room0 and Room3"), UDungeonGraph::FindPath(Room0, Room3, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);
			DUMMY_PATH(Path);
			TestFalse(TEXT("No path should be found between Room3 and Room0"), UDungeonGraph::FindPath(Room3, Room0, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);
			DUMMY_PATH(Path);
			TestFalse(TEXT("No path should be found between Room3 and Room7"), UDungeonGraph::FindPath(Room3, Room7, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);

			// output path has rooms in the correct order
			TestTrue(TEXT("Path should be found between Room0 and Room5"), UDungeonGraph::FindPath(Room0, Room5, &Path));
			TestEqual(TEXT("Path should have 8 rooms"), Path.Num(), 8);
			TestTrue(TEXT("Path should not go through Room3"), !Path.Contains(Room3));
			TestEqual(TEXT("Path's 1st room should be Room0"), Path[0], (const URoom*)Room0);
			TestTrue(TEXT("Path's 2nd room should be Room1"), Path[1] == Room1);
			TestTrue(TEXT("Path's 3rd room should be Room2"), Path[2] == Room2);
			TestTrue(TEXT("Path's 4th room should be Room8"), Path[3] == Room8);
			TestTrue(TEXT("Path's 5th room should be Room9"), Path[4] == Room9);
			TestTrue(TEXT("Path's 6th room should be Room10"), Path[5] == Room10);
			TestTrue(TEXT("Path's 7th room should be Room4"), Path[6] == Room4);
			TestTrue(TEXT("Path's 8th room should be Room5"), Path[7] == Room5);

			// Check pathfind through locked door (not first nor last)
			DUMMY_PATH(Path);
			TestFalse(TEXT("No path should be found between Room0 and Room6"), UDungeonGraph::FindPath(Room0, Room6, &Path));
			TestEqual(TEXT("Path should be empty"), Path.Num(), 0);
			TestTrue(TEXT("Path should be found between Room0 and Room6 (when locked rooms allowed)"), UDungeonGraph::FindPath(Room0, Room6, nullptr, /*IgnoreLocked = */true));

			CLEAN_TEST();
		}
	}

	return true;
}

#undef INIT_TEST
#undef CLEAN_TEST
#undef CREATE_ROOM
#undef CREATE_ROOM_DATA
#undef DUMMY_PATH

#endif //WITH_DEV_AUTOMATION_TESTS
