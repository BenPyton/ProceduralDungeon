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
#include "Engine/DataAsset.h"
#include "DoorType.generated.h"

UCLASS(BlueprintType)
class PROCEDURALDUNGEON_API UDoorType : public UDataAsset
{
	GENERATED_BODY()

public:
	UDoorType();

	// Returns the door size from the door type asset,
	// or the default door size in plugin's settings if no door type defined.
	static FVector GetSize(const UDoorType* DoorType);

protected:
	// Size of the door bounds, only used by the debug draw as a visual hint for designers and artists.
	UPROPERTY(EditInstanceOnly, Category = "Door Type", meta = (ClampMin = 0))
	FVector Size;

#if WITH_EDITORONLY_DATA
	// Just a description, used nowhere.
	UPROPERTY(EditInstanceOnly, Category = "Door Type")
	FText Description;
#endif
};
