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

#pragma once

#include "Framework/Application/SlateApplication.h"
#include "ProceduralDungeonEdTypes.h"

class ARoomLevel;
class URoomData;
class FProceduralDungeonEdMode;
class FProceduralDungeonEdModeToolkit;
template <typename> class SSpinBox;

class SProceduralDungeonEdModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProceduralDungeonEdModeWidget) {}
	SLATE_END_ARGS();

	~SProceduralDungeonEdModeWidget();

	void Construct(const FArguments& InArgs, TSharedRef<FProceduralDungeonEdModeToolkit> InParentToolkit);
	void OnLevelChanged();

protected:
	bool IsValidRoomLevel(FProceduralDungeonEdMode* EdMode = nullptr, TWeakObjectPtr<ARoomLevel>* OutLevel = nullptr) const;
	bool IsValidRoomData(FProceduralDungeonEdMode* EdMode, TWeakObjectPtr<URoomData>* OutData = nullptr, TWeakObjectPtr<ARoomLevel>* OutLevel = nullptr) const;
	bool IsValidRoomData() const { return IsValidRoomData(nullptr); }
	bool MatchingDataLevel(FProceduralDungeonEdMode* EdMode = nullptr) const;
	bool IsDataDirty(FProceduralDungeonEdMode* EdMode) const;
	bool IsDataDirty() const { return IsDataDirty(nullptr); }
	EVisibility ShowDetails() const;
	EVisibility ShowDataDetails() const;
	EVisibility ShowNote() const;
	FText GetDataAssetName() const;
	void OnDataAssetChanged();
	FReply ReparentLevelActor();
	FReply EditData();
	FReply SaveData();
	FReply UpdateSelectedVolumes();
	FReply RemoveInvalidDoors();
	FSlateColor GetSaveButtonColor() const;
	FSlateColor GetReparentButtonColor() const;
	void UpdateErrorText();

	void ResetCachedData();
	void ResetCachedLevel();

	FProceduralDungeonEdMode* GetEditorMode() const;
	void RegisterSelectionDelegate(bool Register);
	void OnSelectedActorsChanged(UObject* NewSelectedObject);

	static FLinearColor GetHighlightButtonColor(const FLinearColor& HighlightColor, const FLinearColor& NormalColor = FLinearColor::White, float Speed = 3.0f);

private:
	TSharedPtr<class SErrorText> Error {nullptr};
	TSharedPtr<class IDetailsView> DataContentWidget {nullptr};
	TWeakPtr<FProceduralDungeonEdModeToolkit> ParentToolkit {nullptr};

	FMargin3D VolumeMargins;

	TSharedPtr<class SBorder> LevelPropertyContainer {nullptr};
	TWeakObjectPtr<URoomData> CachedData {nullptr};
	TWeakObjectPtr<ARoomLevel> CachedLevel {nullptr};
	FDelegateHandle DataDelegateHandle;
	FDelegateHandle LevelDelegateHandle;
	FDelegateHandle SelectionDelegateHandle;
	int32 SelectedVolumeCount {0};
};
