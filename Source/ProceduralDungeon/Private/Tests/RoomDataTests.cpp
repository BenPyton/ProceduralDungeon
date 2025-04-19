/*
 * Copyright (c) 2024-2025 Benoit Pelletier
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
#include "RoomData.h"
#include "DoorType.h"
#include "./Classes/RoomCustomDataChildClasses.h"
#include "UObject/Package.h"
#include "TestUtils.h"
#include "VoxelBounds/VoxelBounds.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRoomDataTests, "ProceduralDungeon.Types.RoomData", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)

	// Utility to create room data
	#define CREATE_ROOM_DATA(Data)          \
		CREATE_DATA_ASSET(URoomData, Data); \
		Data->Doors.Empty();

	#define ADD_DOOR(ROOM, DOOR_POS, DOOR_DIR, DOOR_TYPE) \
		{                                                 \
			FDoorDef Door;                                \
			Door.Position = DOOR_POS;                     \
			Door.Direction = DOOR_DIR;                    \
			Door.Type = DOOR_TYPE;                        \
			ROOM->Doors.Add(Door);                        \
		}

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

	// Test HasDoorOfType and variants
	{
		CREATE_ROOM_DATA(RoomData);
		RoomData->FirstPoint = FIntVector(0, 0, 0);
		RoomData->SecondPoint = FIntVector(1, 1, 1);

		CREATE_DATA_ASSET(UDoorType, DoorA);
		CREATE_DATA_ASSET(UDoorType, DoorB);
		CREATE_DATA_ASSET(UDoorType, DoorC);
		CREATE_DATA_ASSET(UDoorType, DoorD);
		CREATE_DATA_ASSET(UDoorType, DoorE);

		ADD_DOOR(RoomData, FIntVector::ZeroValue, EDoorDirection::North, DoorA.Get());
		ADD_DOOR(RoomData, FIntVector::ZeroValue, EDoorDirection::South, DoorB.Get());
		ADD_DOOR(RoomData, FIntVector::ZeroValue, EDoorDirection::East, DoorC.Get());
		ADD_DOOR(RoomData, FIntVector::ZeroValue, EDoorDirection::West, DoorD.Get());

		// HasDoorOfType
		{
			TestTrue("HasDoorOfType(DoorA)", RoomData->HasDoorOfType(DoorA.Get()));
			TestTrue("HasDoorOfType(DoorB)", RoomData->HasDoorOfType(DoorB.Get()));
			TestTrue("HasDoorOfType(DoorC)", RoomData->HasDoorOfType(DoorC.Get()));
			TestTrue("HasDoorOfType(DoorD)", RoomData->HasDoorOfType(DoorD.Get()));
			TestFalse("HasDoorOfType(DoorE)", RoomData->HasDoorOfType(DoorE.Get()));
			TestFalse("HasDoorOfType(nullptr)", RoomData->HasDoorOfType(nullptr));
		}

		// HasAnyDoorOfType
		{
			TestTrue("HasAnyDoorOfType({DoorA, DoorB, DoorE, nullptr})", RoomData->HasAnyDoorOfType({DoorA.Get(), DoorB.Get(), DoorE.Get(), nullptr}));
			TestTrue("HasAnyDoorOfType({DoorC, DoorD})", RoomData->HasAnyDoorOfType({DoorC.Get(), DoorD.Get()}));
			TestFalse("HasAnyDoorOfType({DoorE, nullptr})", RoomData->HasAnyDoorOfType({DoorE.Get(), nullptr}));
			TestFalse("HasAnyDoorOfType({})", RoomData->HasAnyDoorOfType({}));
		}

		// HasAllDoorOfType
		{
			TestFalse("HasAllDoorOfType({DoorA, DoorB, DoorE, nullptr})", RoomData->HasAllDoorOfType({DoorA.Get(), DoorB.Get(), DoorE.Get(), nullptr}));
			TestTrue("HasAllDoorOfType({DoorC, DoorD})", RoomData->HasAllDoorOfType({DoorC.Get(), DoorD.Get()}));
			TestFalse("HasAllDoorOfType({DoorE, nullptr})", RoomData->HasAllDoorOfType({DoorE.Get(), nullptr}));
			TestTrue("HasAllDoorOfType({})", RoomData->HasAllDoorOfType({}));
		}
	}

	// Test HasCustomData and variants
	{
		CREATE_ROOM_DATA(RoomData);
		RoomData->FirstPoint = FIntVector(0, 0, 0);
		RoomData->SecondPoint = FIntVector(1, 1, 1);

		RoomData->CustomData.Add(UCustomDataA::StaticClass());
		RoomData->CustomData.Add(UCustomDataB::StaticClass());

		// HasCustomData
		{
			TestTrue("HasCustomData(CustomDataA)", RoomData->HasCustomData(UCustomDataA::StaticClass()));
			TestTrue("HasCustomData(CustomDataB)", RoomData->HasCustomData(UCustomDataB::StaticClass()));
			TestFalse("HasCustomData(CustomDataC)", RoomData->HasCustomData(UCustomDataC::StaticClass()));
			TestFalse("HasCustomData(nullptr)", RoomData->HasCustomData(nullptr));
		}

		// HasAnyCustomData
		{
			TestTrue("HasAnyCustomData({CustomDataA, CustomDataB})", RoomData->HasAnyCustomData({UCustomDataA::StaticClass(), UCustomDataB::StaticClass()}));
			TestTrue("HasAnyCustomData({CustomDataA, CustomDataC})", RoomData->HasAnyCustomData({UCustomDataA::StaticClass(), UCustomDataC::StaticClass()}));
			TestFalse("HasAnyCustomData({nullptr, CustomDataC})", RoomData->HasAnyCustomData({nullptr, UCustomDataC::StaticClass()}));
			TestFalse("HasAnyCustomData({})", RoomData->HasAnyCustomData({}));
		}

		// HasAllCustomData
		{
			TestTrue("HasAllCustomData({CustomDataA, CustomDataB})", RoomData->HasAllCustomData({UCustomDataA::StaticClass(), UCustomDataB::StaticClass()}));
			TestFalse("HasAllCustomData({CustomDataA, CustomDataC})", RoomData->HasAllCustomData({UCustomDataA::StaticClass(), UCustomDataC::StaticClass()}));
			TestFalse("HasAllCustomData({nullptr, CustomDataC})", RoomData->HasAllCustomData({nullptr, UCustomDataC::StaticClass()}));
			TestTrue("HasAllCustomData({})", RoomData->HasAllCustomData({}));
		}
	}

	// Test Size and Volume
	{
		// Should have Size=(1,1,1) and Volume=1
		CREATE_ROOM_DATA(RoomDataA);
		RoomDataA->FirstPoint = FIntVector(0, 1, 0);
		RoomDataA->SecondPoint = FIntVector(1, 0, 1);

		// Should have Size=(2,1,1) and Volume=2
		CREATE_ROOM_DATA(RoomDataB);
		RoomDataB->FirstPoint = FIntVector(-1, 1, 0);
		RoomDataB->SecondPoint = FIntVector(1, 0, 1);

		// Should have Size=(2,2,1) and Volume=4
		CREATE_ROOM_DATA(RoomDataC);
		RoomDataC->FirstPoint = FIntVector(-1, 1, 0);
		RoomDataC->SecondPoint = FIntVector(1, -1, 1);

		// Should have Size=(2,2,2) and Volume=8
		CREATE_ROOM_DATA(RoomDataD);
		RoomDataD->FirstPoint = FIntVector(-1, 1, -1);
		RoomDataD->SecondPoint = FIntVector(1, -1, 1);

		// GetSize
		{
			TestEqual("RoomDataA->GetSize() == {1,1,1}", RoomDataA->GetSize(), FIntVector {1, 1, 1});
			TestEqual("RoomDataB->GetSize() == {2,1,1}", RoomDataB->GetSize(), FIntVector {2, 1, 1});
			TestEqual("RoomDataC->GetSize() == {2,2,1}", RoomDataC->GetSize(), FIntVector {2, 2, 1});
			TestEqual("RoomDataD->GetSize() == {2,2,2}", RoomDataD->GetSize(), FIntVector {2, 2, 2});
		}

		// GetVolume
		{
			TestEqual("RoomDataA->GetVolume() == 1", RoomDataA->GetVolume(), 1);
			TestEqual("RoomDataB->GetVolume() == 2", RoomDataB->GetVolume(), 2);
			TestEqual("RoomDataC->GetVolume() == 4", RoomDataC->GetVolume(), 4);
			TestEqual("RoomDataD->GetVolume() == 8", RoomDataD->GetVolume(), 8);
		}
	}

	// Test GetVoxelBounds
	{
		CREATE_ROOM_DATA(RoomDataA);
		RoomDataA->FirstPoint = FIntVector(0, 0, 0);
		RoomDataA->SecondPoint = FIntVector(1, 1, 1);
		RoomDataA->Doors.Add({{0, 0, 0}, EDoorDirection::North});

		// Should have one cell at (0,0,0) with a door at (0,0,0)[North]
		{
			FVoxelBounds ExpectedBounds;
			ExpectedBounds.AddCell({0, 0, 0});

			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			FVoxelBounds ConvertedBounds = RoomDataA->GetVoxelBounds();
			TestEqual("RoomDataA->GetVoxelBounds() == ExpectedBounds", ConvertedBounds, ExpectedBounds);
		}

		CREATE_ROOM_DATA(RoomDataB);
		RoomDataB->FirstPoint = FIntVector(-1, 0, 0);
		RoomDataB->SecondPoint = FIntVector(2, 1, 1);
		RoomDataB->Doors.Add({{0, 0, 0}, EDoorDirection::South});
		RoomDataB->Doors.Add({{1, 0, 0}, EDoorDirection::East});

		// Should have 3 cells at (-1,0,0), (0,0,0), (1,0,0) with doors at (0,0,0)[South] and (1,0,0)[East]
		{
			FVoxelBounds ExpectedBounds;
			ExpectedBounds.AddCell({-1, 0, 0});
			ExpectedBounds.AddCell({0, 0, 0});
			ExpectedBounds.AddCell({1, 0, 0});

			ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({-1, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			ExpectedBounds.SetCellConnection({1, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({1, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({1, 0, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door));
			ExpectedBounds.SetCellConnection({1, 0, 0}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({1, 0, 0}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			FVoxelBounds ConvertedBounds = RoomDataB->GetVoxelBounds();
			TestEqual("RoomDataB->GetVoxelBounds() == ExpectedBounds", ConvertedBounds, ExpectedBounds);
		}

		CREATE_ROOM_DATA(RoomDataC);
		RoomDataC->FirstPoint = FIntVector(0, 0, -1);
		RoomDataC->SecondPoint = FIntVector(1, 1, 2);
		RoomDataC->Doors.Add({{0, 0, 0}, EDoorDirection::North});
		RoomDataC->Doors.Add({{0, 0, 1}, EDoorDirection::South});

		// Should have 3 cells at (0,0,-1), (0,0,0), (0,0,1) with doors at (0,0,0)[North] and (0,0,1)[South]
		{
			FVoxelBounds ExpectedBounds;
			ExpectedBounds.AddCell({0, 0, -1});
			ExpectedBounds.AddCell({0, 0, 0});
			ExpectedBounds.AddCell({0, 0, 1});

			ExpectedBounds.SetCellConnection({0, 0, -1}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, -1}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, -1}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, -1}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, -1}, FVoxelBounds::EDirection::Down, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 0}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::North, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::West, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::South, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Door));
			ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::East, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));
			ExpectedBounds.SetCellConnection({0, 0, 1}, FVoxelBounds::EDirection::Up, FVoxelBoundsConnection(FVoxelBoundsConnection::EType::Wall));

			FVoxelBounds ConvertedBounds = RoomDataC->GetVoxelBounds();
			TestEqual("RoomDataC->GetVoxelBounds() == ExpectedBounds", ConvertedBounds, ExpectedBounds);
		}
	}

	return true;
}

	#undef CREATE_DATA_ASSET
	#undef CREATE_ROOM_DATA
	#undef ADD_DOOR

#endif //WITH_DEV_AUTOMATION_TESTS
