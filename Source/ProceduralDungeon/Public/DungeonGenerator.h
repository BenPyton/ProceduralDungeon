/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Benoit Pelletier
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

#pragma once

#include "CoreMinimal.h"
#include "DungeonGeneratorBase.h"
#include "BoundsParams.h"
#include "DungeonGenerator.generated.h"

class IReadOnlyRoom;

// This is the main actor of the plugin. The dungeon generator is responsible to generate dungeons and replicate them over the network. 
UCLASS(Blueprintable, ClassGroup = "Procedural Dungeon", HideFunctions = "StartNewDungeon,FinalizeDungeon,CreateRoomInstance,TryPlaceRoom,AddRoomToDungeon", meta = (KismetHideOverrides = "CreateDungeon", HideFunctions = "CreateRoomInstance,TryPlaceRoom,AddRoomToDungeon"))
class PROCEDURALDUNGEON_API ADungeonGenerator : public ADungeonGeneratorBase
{
	GENERATED_BODY()

public:
	ADungeonGenerator();

protected:
	//~ Begin ADungeonGeneratorBase Interface
	virtual bool CreateDungeon_Implementation() override;
	//~ End ADungeonGeneratorBase Interface

public:
	// ===== Methods that should be overriden in blueprint =====

	// Return the RoomData you want as root of the dungeon generation
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose First Room"))
	URoomData* ChooseFirstRoomData();

	/** Return the RoomData that will be connected to the Current Room
	* @param CurrentRoom [DEPRECATED] Use CurrentRoomInstance->GetRoomData instead.
	* @param CurrentRoomInstance The room instance to which the generator will connect the next room. This interface allows access only to some data.
	* @param DoorData The door of the CurrentRoom on which the next room will be connected (its location in room units, its orientation and its type).
	* @param DoorIndex The index of the door used on the next room to connect to the CurrentRoom.
	* Use -1 for a random (compatible) door, or the door index from the RoomData door array (0 is the first door).
	* WARNING: If the RandomDoor boolean of the RoomData is checked, then it will be considered -1 whatever you set here.
	* @return The room data asset used to instantiate the new room instance (must not be null)
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Choose Next Room", ReturnDisplayName="Room Data", AutoCreateRefTerm = "DoorIndex"))
	URoomData* ChooseNextRoomData(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex);

	// Condition to validate a dungeon Generation
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Is Valid Dungeon"))
	bool IsValidDungeon();

	// Condition to continue or stop adding room to the dungeon
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Generator", meta = (DisplayName = "Continue To Add Room"))
	bool ContinueToAddRoom();

	// ===== Utility functions you can use in blueprint =====

	// Return true if a specific RoomData is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyRoomData(URoomData* RoomData);

	// Return true if at least one of the RoomData from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList);

	// Return the number of a specific RoomData in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountRoomData(URoomData* RoomData);

	// Return the total number of RoomData in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountTotalRoomData(TArray<URoomData*> RoomDataList);

	// Return true if a specific RoomData type is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyRoomType(TSubclassOf<URoomData> RoomType);

	// Return true if at least one of the RoomData type from the list provided is already in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	bool HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Return the number of a specific RoomData type in the dungeon
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountRoomType(TSubclassOf<URoomData> RoomType);

	// Return the total number of RoomData type in the dungeon from the list provided
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeon Generator", meta = (DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList);

	// Returns the current number of room in the generated dungeon.
	UFUNCTION(BlueprintPure, Category = "Dungeon Generator", meta = (DisplayName = "Room Count", CompactNodeTitle = "Room Count", DeprecatedFunction, DeprecationMessage = "Use the same function from the Rooms variable."))
	int GetNbRoom();

	// Must be called in "Choose Next Room" function to be used.
	// Tell explicitely the generator we don't want to place a room for a specific door.
	// No error will be thrown when returning a null room data and no further room placement tries occur for this door (skip directly to the next door).
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void DiscardRoom() { bDiscardRoom = true; }

private:
	// Adds some new rooms linked to ParentRoom into Rooms list output
	// AddedRooms contains only the new rooms added to Rooms list
	// Returns true if the dungeon should keep adding new rooms
	bool AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms);

public:
	// In which order the dungeon generate rooms.
	// Depth First: Dungeon will use the last generated room to place the next one. Resulting in a more linear dungeon.
	// Breadth First: Dungeon will generate a room at each door of the current one before going to the next room. Resulting in a more spread dungeon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	EGenerationType GenerationType;

	// If ticked, newly placed room will check if any door is aligned with another room, and if so will connect them.
	// If unticked, only the doors between CurrentRoom and NextRoom (in the function ChooseNextRoom) will be connected.
	// (will only have effect if the deprecated CanLoop in the plugin settings is ticked too, until it is removed in a future version)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	bool bCanLoop {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation")
	FBoundsParams DungeonLimits;

	// If true, returning null in ChooseNextRoom is the same as calling DiscardRoom.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Generation", AdvancedDisplay)
	bool bAutoDiscardRoomIfNull = false;

	// Flag to explicitely tell we don't want to place a room.
	bool bDiscardRoom = false;
};
