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

UCLASS()
class PROCEDURALDUNGEON_API UDungeonBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities|Procedural Dungeon")
	static bool IsDoorOfType(const TSubclassOf<class ADoor> DoorClass, const class UDoorType* DoorType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities", meta = (DisplayName = "Equal (Data Table Row Handle)", CompactNodeTitle = "=="))
	static bool CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B);

	// ===== DoorDirection Math Utility Functions =====

	// True if the value is set (either North, South, East, West)
	// False otherwise
	UFUNCTION(BlueprintCallable, Category = "Math|Door Direction", meta = (DisplayName = "Is Valid", ExpandBoolAsExecs = "ReturnValue"))
	static bool DoorDirection_Valid(const EDoorDirection& A) { return !!A; }

	// Addition (A + B)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Direction + Direction", CompactNodeTitle = "+", AutoCreateRefTerm = "A,B"))
	static EDoorDirection DoorDirection_Add(const EDoorDirection& A, const EDoorDirection& B) { return A + B; }

	// Subtraction (A - B)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Direction - Direction", CompactNodeTitle = "-", AutoCreateRefTerm = "A,B"))
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Door Direction", meta = (DisplayName = "Opposite", CompactNodeTitle = "Opposite", AutoCreateRefTerm = "A"))
	static EDoorDirection DoorDirection_Opposite(const EDoorDirection& A) { return ~A; }
};
