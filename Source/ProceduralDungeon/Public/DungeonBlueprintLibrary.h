/*
 * MIT License
 *
 * Copyright (c) 2023 Benoit Pelletier
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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralDungeonTypes.h"
#include "Engine/DataTable.h"
#include "DungeonBlueprintLibrary.generated.h"

class URoom;
class URoomCustomData;

UCLASS()
class PROCEDURALDUNGEON_API UDungeonBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Utilities|Procedural Dungeon")
	static bool IsDoorOfType(const TSubclassOf<class ADoor> DoorClass, const class UDoorType* DoorType);

	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (DisplayName = "Equal (Data Table Row Handle)", CompactNodeTitle = "=="))
	static bool CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B);

	// Returns the room instance the actor is in.
	// If the actor is spawned at runtime or the owning level is not a room level, returns null.
	UFUNCTION(BlueprintPure, Category = "Utilities|Procedural Dungeon", meta = (DefaultToSelf = "Target"))
	static URoom* GetOwningRoom(const AActor* Target);

	// Returns the first RoomCustomData of the provided type in the owning room.
	// If no owning room or no custom data of this type, returns null.
	UFUNCTION(BlueprintCallable, Category = "Utilities|Procedural Dungeon", meta = (DefaultToSelf = "Target", ExpandBoolAsExecs = "ReturnValue", DeterminesOutputType = "CustomDataClass", DynamicOutputParam = "CustomData"))
	static bool GetOwningRoomCustomData(const AActor* Target, TSubclassOf<URoomCustomData> CustomDataClass, URoomCustomData*& CustomData);

	// ===== DoorDirection Math Utility Functions =====

	// True if the value is set (either North, South, East, West)
	// False otherwise
	UFUNCTION(BlueprintCallable, Category = "Math|Door Direction", meta = (DisplayName = "Is Valid", ExpandBoolAsExecs = "ReturnValue"))
	static bool DoorDirection_Valid(const EDoorDirection& A) { return !!A; }

	// Addition (A + B)
	UFUNCTION(BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Direction + Direction", CompactNodeTitle = "+", AutoCreateRefTerm = "A,B"))
	static EDoorDirection DoorDirection_Add(const EDoorDirection& A, const EDoorDirection& B) { return A + B; }

	// Subtraction (A - B)
	UFUNCTION(BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Direction - Direction", CompactNodeTitle = "-", AutoCreateRefTerm = "A,B"))
	static EDoorDirection DoorDirection_Sub(const EDoorDirection& A, const EDoorDirection& B) { return A - B; }

	// Increment the direction and set it
	UFUNCTION(BlueprintCallable, Category = "Math|Door Direction", meta = (DisplayName = "Increment Door Direction", CompactNodeTitle = "++"))
	static EDoorDirection& DoorDirection_Increment(UPARAM(ref) EDoorDirection& A) { return ++A; }

	// Decrement the direction and set it
	UFUNCTION(BlueprintCallable, Category = "Math|Door Direction", meta = (DisplayName = "Decrement Door Direction", CompactNodeTitle = "--"))
	static EDoorDirection& DoorDirection_Decrement(UPARAM(ref) EDoorDirection& A) { return --A; }

	// Negate the direction and set it (same as North - Direction)
	UFUNCTION(BlueprintCallable, Category = "Math|Door Direction", meta = (DisplayName = "Negate Door Direction", CompactNodeTitle = "-"))
	static EDoorDirection& DoorDirection_Negate(UPARAM(ref) EDoorDirection& A) { A = -A;  return A; }

	// Transforms North into South and East into West (and vice versa)
	UFUNCTION(BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Opposite", CompactNodeTitle = "Opposite", AutoCreateRefTerm = "A"))
	static EDoorDirection DoorDirection_Opposite(const EDoorDirection& A) { return ~A; }

	// ===== Plugin Settings Accessors =====

	// Returns the room unit size in unreal units
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Get Room Unit"))
	static FVector Settings_RoomUnit();

	// Returns the default door type's size
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Get Default Door Size"))
	static FVector Settings_DefaultDoorSize();

	// Returns the room offset as a percentage of the height of a room unit
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Get Door Offset"))
	static float Settings_DoorOffset();

	// Returns true if the plugin's occlusion system is enabled
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Is Occlusion Culling Enabled"))
	static bool Settings_OcclusionCulling();

	// Enable/disable the plugin's occlusion system
	UFUNCTION(BlueprintCallable, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Enable Occlusion Culling"))
	static void Settings_SetOcclusionCulling(bool Enable);

	// Returns the number of visible room from the player's room (1 mean only the player room is visible)
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Get Occlusion Culling Distance"))
	static int32 Settings_OcclusionDistance();

	// Set the number of visible rooms from the player's room (1 mean only the player room is visible)
	UFUNCTION(BlueprintCallable, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Set Occlusion Culling Distance"))
	static void Settings_SetOcclusionDistance(int32 Distance);

	// Returns true if actors with a RoomVisibility component should have their visibility toggled with the rooms
	UFUNCTION(BlueprintPure, Category = "Procedural Dungeon|Settings", meta = (DisplayName = "Should Dynamic Actors Be Occluded"))
	static bool Settings_OccludeDynamicActors();

	// ===== Gameplay Utility Functions =====

	// Set player to spectate
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Player", meta = (DefaultToSelf="Controller"))
	static void Spectate(APlayerController* Controller, bool DestroyPawn = true);

};
