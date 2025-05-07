/*
 * Copyright (c) 2023-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Misc/AutomationTest.h"
#include "DungeonGraph.h"
#include "Room.h"
#include "RoomData.h"
#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonGraphTest, "ProceduralDungeon.Types.DungeonGraph", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)

	#define INIT_TEST(Graph) \
		TStrongObjectPtr<UDungeonGraph> Graph(NewObject<UDungeonGraph>(GetTransientPackage(), #Graph));

	#define CLEAN_TEST() \
		Graph->Clear();

	// Utility to create and initialize a room
	#define CREATE_ROOM(Name, RoomDataPtr)                      \
		URoom* Name = NewObject<URoom>();                       \
		Name->Init(RoomDataPtr.Get(), nullptr, Graph->Count()); \
		Graph->AddRoom(Name);

	// Utility to create room data
	#define CREATE_ROOM_DATA(Data)          \
		CREATE_DATA_ASSET(URoomData, Data); \
		Data->Doors.Empty();

	// Utility to create a non-empty path
	#define DUMMY_PATH(Path) \
		Path.Empty();        \
		Path.Add(nullptr);

bool FDungeonGraphTest::RunTest(const FString& Parameters)
{
	{
		// Creating data assets
		CREATE_ROOM_DATA(DA_A);
		CREATE_ROOM_DATA(DA_B);
		CREATE_ROOM_DATA(DA_C);
		CREATE_ROOM_DATA(DA_D);

		DA_A->Doors.Add({{0, 0, 0}, EDoorDirection::South});

		DA_B->Doors.Add({{0, 0, 0}, EDoorDirection::East});
		DA_B->Doors.Add({{0, 0, 0}, EDoorDirection::West});

		DA_C->Doors.Add({{0, 0, 0}, EDoorDirection::North});
		DA_C->Doors.Add({{0, 0, 0}, EDoorDirection::South});
		DA_C->Doors.Add({{0, 0, 0}, EDoorDirection::East});
		DA_C->Doors.Add({{0, 0, 0}, EDoorDirection::West});

		DA_D->SecondPoint = {1, 1, 2};
		DA_D->Doors.Add({{0, 0, 0}, EDoorDirection::North});
		DA_D->Doors.Add({{0, 0, 1}, EDoorDirection::North});

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
			Graph->Connect(Room0, 0, Room1, 1);
			Graph->Connect(Room1, 0, Room2, 1);
			Graph->Connect(Room2, 0, Room3, 1);
			Graph->Connect(Room3, 0, Room4, 1);
			Graph->Connect(Room4, 0, Room5, 1);

			// second line
			Graph->Connect(Room6, 0, Room7, 0);
			Graph->Connect(Room8, 0, Room9, 1);
			Graph->Connect(Room9, 0, Room10, 0);

			// transversal
			Graph->Connect(Room1, 2, Room7, 1);
			Graph->Connect(Room2, 2, Room8, 1);
			Graph->Connect(Room4, 2, Room10, 1);

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
			TestTrue(TEXT("Path should be found between Room0 and Room6 (when locked rooms allowed)"), UDungeonGraph::FindPath(Room0, Room6, nullptr, /*IgnoreLocked = */ true));

			CLEAN_TEST();
		}

		// Test Voxel Bounds Conversions
		{
			INIT_TEST(Graph);

			// first floor		second floor
			// C - B - A		C - C
			// |				|
			// D				D

			CREATE_ROOM(Room0, DA_C);
			CREATE_ROOM(Room1, DA_B);
			CREATE_ROOM(Room2, DA_A);
			CREATE_ROOM(Room3, DA_D);
			CREATE_ROOM(Room4, DA_C);
			CREATE_ROOM(Room5, DA_C);

			Room1->Position = {0, 1, 0};
			Room2->Position = {0, 2, 0};
			Room2->Direction = EDoorDirection::East;
			Room3->Position = {-1, 0, 0};
			Room4->Position = {0, 0, 1};
			Room5->Position = {0, 1, 1};

			// Room positions are modified manually, so we need to explicitely rebuild bounds
			Graph->RebuildBounds();

			// Check Room0 voxel bounds conversion
			{
				FVoxelBounds ExpectedBounds;
				ExpectedBounds.AddCell({0, 0, 0});
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room0->GetVoxelBounds();
				TestEqual(TEXT("Room0 bounds should be as expected"), ConvertedBounds, ExpectedBounds);
			}

			// Check Room1 voxel bounds conversion
			{
				FVoxelBounds ExceptedBounds;
				ExceptedBounds.AddCell({0, 1, 0});
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExceptedBounds.SetCellConnection({0, 1, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room1->GetVoxelBounds();
				TestEqual(TEXT("Room1 bounds should be as expected"), ConvertedBounds, ExceptedBounds);
			}

			// Check Room2 voxel bounds conversion
			{
				FVoxelBounds ExpectedBounds;
				ExpectedBounds.AddCell({0, 2, 0});
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 2, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room2->GetVoxelBounds();
				TestEqual(TEXT("Room2 bounds should be as expected"), ConvertedBounds, ExpectedBounds);
			}

			// Check Room3 voxel bounds conversion
			{
				FVoxelBounds ExpectedBounds;
				ExpectedBounds.AddCell({-1, 0, 0});
				ExpectedBounds.AddCell({-1, 0, 1});
				ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 1}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({-1, 0, 1}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 1}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 1}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({-1, 0, 1}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room3->GetVoxelBounds();
				TestEqual(TEXT("Room3 bounds should be as expected"), ConvertedBounds, ExpectedBounds);
			}

			// Check Room4 voxel bounds conversion
			{
				FVoxelBounds ExpectedBounds;
				ExpectedBounds.AddCell({0, 0, 1});
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room4->GetVoxelBounds();
				TestEqual(TEXT("Room4 bounds should be as expected"), ConvertedBounds, ExpectedBounds);
			}

			// Check Room5 voxel bounds conversion
			{
				FVoxelBounds ExpectedBounds;
				ExpectedBounds.AddCell({0, 1, 1});
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Door));
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));
				ExpectedBounds.SetCellConnection({0, 1, 1}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(EVoxelBoundsConnectionType::Wall));

				FVoxelBounds ConvertedBounds = Room5->GetVoxelBounds();
				TestEqual(TEXT("Room5 bounds should be as expected"), ConvertedBounds, ExpectedBounds);
			}
		}

		// FilterAndSort Test
		{
			INIT_TEST(Graph);

			// first floor		second floor
			// C - B - A		C - C
			// |				|
			// D				D

			CREATE_ROOM_DATA(DA_E);
			DA_E->SecondPoint = {1, 2, 1};
			DA_E->Doors.Add({{0, 1, 0}, EDoorDirection::North});

			CREATE_ROOM(Room0, DA_C);
			CREATE_ROOM(Room1, DA_B);
			CREATE_ROOM(Room2, DA_A);
			CREATE_ROOM(Room3, DA_D);
			CREATE_ROOM(Room4, DA_C);
			CREATE_ROOM(Room5, DA_C);

			Room1->Position = {0, 1, 0};
			Room2->Position = {0, 2, 0};
			Room2->Direction = EDoorDirection::East;
			Room3->Position = {-1, 0, 0};
			Room4->Position = {0, 0, 1};
			Room5->Position = {0, 1, 1};

			// Room positions are modified manually, so we need to explicitely rebuild bounds
			Graph->RebuildBounds();

			const TArray<URoomData*> RoomList = {DA_A.Get(), DA_D.Get(), DA_E.Get()};
			TArray<FRoomCandidate> SortedRooms;

			{
				FDoorDef FromDoor = {{0, 0, 0}, EDoorDirection::North};
				bool bHasCandidates = Graph->FilterAndSortRooms(RoomList, FromDoor, SortedRooms);
				TestTrue(TEXT("There should be candidates"), bHasCandidates);
				TestEqual(TEXT("There should be 4 candidates"), SortedRooms.Num(), 4);
				TestEqual(TEXT("RoomData D should be the best candidate"), SortedRooms[0].Data, DA_D.Get());
				TestEqual(TEXT("RoomData D Door index 0 should be the best candidate"), SortedRooms[0].DoorIndex, 0);
				TestEqual(TEXT("RoomData A should be the worst candidate"), SortedRooms[3].Data, DA_A.Get());
			}

			{
				FDoorDef FromDoor = {{0, 1, 1}, EDoorDirection::South};
				bool bHasCandidates = Graph->FilterAndSortRooms(RoomList, FromDoor, SortedRooms);
				TestTrue(TEXT("There should be candidates"), bHasCandidates);
				TestEqual(TEXT("There should be 3 candidates"), SortedRooms.Num(), 3);
				TestEqual(TEXT("RoomData D should be the best candidate"), SortedRooms[0].Data, DA_D.Get());
				TestEqual(TEXT("RoomData D DoorIndex 0 should be the best candidate"), SortedRooms[0].DoorIndex, 0);
				TestEqual(TEXT("RoomData D should be the worst candidate"), SortedRooms[2].Data, DA_D.Get());
				TestEqual(TEXT("RoomData D DoorIndex 1 should be the worst candidate"), SortedRooms[2].DoorIndex, 1);
			}

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
