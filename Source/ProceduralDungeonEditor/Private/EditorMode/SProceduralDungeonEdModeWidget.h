/*
 * Copyright (c) 2023-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
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
