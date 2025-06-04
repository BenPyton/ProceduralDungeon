// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

	UFUNCTION(BlueprintPure, Category = "DoorDef", meta = (DisplayName = "Opposite", CompactNodeTitle = "Opposite"))
	static FDoorDef DoorDef_GetOpposite(const FDoorDef& DoorDef);

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

	// Convert a DoorDirection enum value into a unit IntVector pointing in that direction.
	UFUNCTION(BlueprintPure, Category = "Conversion|Door Direction", meta = (BlueprintAutocast, DisplayName = "To Int Vector", AutoCreateRefTerm = "A"))
	static FIntVector DoorDirection_ToIntVector(const EDoorDirection& A) { return ToIntVector(A); }

	// Convert a DoorDirection enum value into a unit IntVector pointing in that direction.
	UFUNCTION(BlueprintPure, Category = "Conversion|Door Direction", meta = (BlueprintAutocast, DisplayName = "To Angle", AutoCreateRefTerm = "A"))
	static float DoorDirection_ToAngle(const EDoorDirection& A) { return ToAngle(A); }

	// ===== Dungeon Math Transform =====

	// Returns the neighbor at the provided direction.
	// Same as Vector + ToIntVector(Direction)
	UFUNCTION(BlueprintPure, Category = "Math|Transform", meta = (DisplayName = "Next (Int Vector)"))
	static FIntVector IntVector_Next(const FIntVector& Vector, const EDoorDirection& Direction);

	UFUNCTION(BlueprintPure, Category = "Math|Transform", meta = (DisplayName = "Rotate (Int Vector)"))
	static FIntVector IntVector_Rotate(const FIntVector& Vector, const EDoorDirection& Direction);

	// Transform a cell position from local coordinates into the dungeon coordinates
	UFUNCTION(BlueprintPure, Category = "Math|Dungeon", meta = (DisplayName = "Transform Position (Dungeon)", AutoCreateRefTerm = "Rotation"))
	static FIntVector Dungeon_TransformPosition(const FIntVector& LocalPos, const FIntVector& Translation, const EDoorDirection& Rotation);

	// Inverse transform a cell position from the dungeon coordinates into a local coordinates
	UFUNCTION(BlueprintPure, Category = "Math|Dungeon", meta = (DisplayName = "Inverse Transform Position (Dungeon)", AutoCreateRefTerm = "Rotation"))
	static FIntVector Dungeon_InverseTransformPosition(const FIntVector& DungeonPos, const FIntVector& Translation, const EDoorDirection& Rotation);

	// Transform a DoorDef structure from local coordinates into the dungeon coordinates
	UFUNCTION(BlueprintPure, Category = "Math|Dungeon", meta = (DisplayName = "Transform DoorDef (Dungeon)", AutoCreateRefTerm = "Rotation"))
	static FDoorDef Dungeon_TransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation);

	// Inverse transform a DoorDef structure from the dungeon coordinates into a local coordinates
	UFUNCTION(BlueprintPure, Category = "Math|Dungeon", meta = (DisplayName = "Inverse Transform DoorDef (Dungeon)", AutoCreateRefTerm = "Rotation"))
	static FDoorDef Dungeon_InverseTransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation);

	// ===== Int Vector Operators =====

	UFUNCTION(BlueprintPure, Category = "Utilities|Operators", meta = (DisplayName = "Add (Int Vector)", CompactNodeTitle = "+", CallableWithoutWorldContext, CommutativeAssociativeBinaryOperator))
	static FIntVector IntVector_Add(const FIntVector& A, const FIntVector& B);

	UFUNCTION(BlueprintPure, Category = "Utilities|Operators", meta = (DisplayName = "Subtract (Int Vector)", CompactNodeTitle = "-", CallableWithoutWorldContext, CommutativeAssociativeBinaryOperator))
	static FIntVector IntVector_Subtract(const FIntVector& A, const FIntVector& B);

	UFUNCTION(BlueprintPure, Category = "Utilities|Operators", meta = (DisplayName = "Equal (Int Vector)", Keywords = "==", CompactNodeTitle = "==", CallableWithoutWorldContext))
	static bool IntVector_Equal(const FIntVector& A, const FIntVector& B);

	UFUNCTION(BlueprintPure, Category = "Utilities|Operators", meta = (DisplayName = "Not Equal (Int Vector)", Keywords = "!=", CompactNodeTitle = "!=", CallableWithoutWorldContext))
	static bool IntVector_NotEqual(const FIntVector& A, const FIntVector& B);

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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Player", meta = (DefaultToSelf = "Controller"))
	static void Spectate(APlayerController* Controller, bool DestroyPawn = true);
};
