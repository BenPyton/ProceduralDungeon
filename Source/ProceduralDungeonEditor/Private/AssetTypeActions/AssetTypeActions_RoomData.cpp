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

#include "AssetTypeActions_RoomData.h"
#include "RoomData.h"
#include "Modules/ModuleManager.h"
#include "ProceduralDungeonEditor.h"

#define LOCTEXT_NAMESPACE "ProceduralDungeonEditor"

FAssetTypeActions_RoomData::FAssetTypeActions_RoomData()
{
}

FText FAssetTypeActions_RoomData::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_RoomData", "Room Data");
}

UClass* FAssetTypeActions_RoomData::GetSupportedClass() const
{
	return URoomData::StaticClass();
}

FColor FAssetTypeActions_RoomData::GetTypeColor() const
{
	return FColor(255, 50, 128);
}

uint32 FAssetTypeActions_RoomData::GetCategories()
{
	return EAssetTypeCategories::None; // Defined in the Factory
}

bool FAssetTypeActions_RoomData::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

#undef LOCTEXT_NAMESPACE
