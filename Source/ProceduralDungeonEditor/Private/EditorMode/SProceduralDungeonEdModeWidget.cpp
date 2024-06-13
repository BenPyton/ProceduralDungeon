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

#include "SProceduralDungeonEdModeWidget.h"
#include "EditorModeManager.h"
#include "EditorStyleSet.h"
#include "ISinglePropertyView.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SErrorText.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Input/SSpinBox.h"
#include "FileHelpers.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/EngineVersionComparison.h"
#include "GameFramework/Volume.h"
#include "Builders/CubeBuilder.h"
#include "Engine/Selection.h"
#include "ProceduralDungeonEdLog.h"
#include "ProceduralDungeonEditor.h"
#include "ProceduralDungeonEdMode.h"
#include "ProceduralDungeonEdModeToolkit.h"
#include "ProceduralDungeonEditorSettings.h"
#include "Tools/ProceduralDungeonEditorTool.h"
#include "Room.h" // TODO: remove the need to include Room.h when including RoomLevel.h
#include "RoomLevel.h"
#include "RoomData.h"

#if UE_VERSION_OLDER_THAN(5, 1, 0)
using StyleProvider = FEditorStyle;
#else
using StyleProvider = FAppStyle;
#endif

void SProceduralDungeonEdModeWidget::Construct(const FArguments& InArgs, TSharedRef<FProceduralDungeonEdModeToolkit> InParentToolkit)
{
	ParentToolkit = InParentToolkit;
	FProceduralDungeonEdMode* EdMode = InParentToolkit->GetDungeonEditorMode();
	FText LevelName = FText::FromString(GetNameSafe(EdMode->GetWorld()));

	FSlateFontInfo TitleFont = StyleProvider::GetFontStyle("DetailsView.CategoryFontStyle");
	TitleFont.Size = 24;

	FSlateFontInfo SubTitleFont = StyleProvider::GetFontStyle("DetailsView.CategoryFontStyle");
	SubTitleFont.Size = 16;

	const UProceduralDungeonEditorSettings* EditorSettings = GetDefault<UProceduralDungeonEditorSettings>();
	VolumeMargins = EditorSettings->DefaultMargins;

	TSharedPtr<SScrollBox> DataScrollBox = nullptr;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Top)
		.Padding(5.f)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LevelName)
			.Justification(ETextJustify::Center)
			.AutoWrapText(true)
			.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			.Font(TitleFont)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SAssignNew(LevelPropertyContainer, SBorder)
			.Visibility(this, &SProceduralDungeonEdModeWidget::ShowDetails)
			.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
		]
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SAssignNew(Error, SErrorText)
		]
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		[
			SNew(SBorder)
			.BorderImage(StyleProvider::GetBrush("DetailsView.CollapsedCategory"))
			.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
			.Padding(5.0f)
			.Visibility(this, &SProceduralDungeonEdModeWidget::ShowDataDetails)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Top)
				.AutoHeight()
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					[
						SNew(STextBlock)
						.Text(this, &SProceduralDungeonEdModeWidget::GetDataAssetName)
						.Justification(ETextJustify::Center)
						.Font(SubTitleFont)
					]
					+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Edit")))
						.OnClicked(this, &SProceduralDungeonEdModeWidget::EditData)
						.IsEnabled(this, &SProceduralDungeonEdModeWidget::IsValidRoomData)
						.HAlign(EHorizontalAlignment::HAlign_Center)
					]
					+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Save")))
						.OnClicked(this, &SProceduralDungeonEdModeWidget::SaveData)
						.IsEnabled(this, &SProceduralDungeonEdModeWidget::IsDataDirty)
						.ButtonColorAndOpacity(this, &SProceduralDungeonEdModeWidget::GetSaveButtonColor)
						.HAlign(EHorizontalAlignment::HAlign_Center)
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(0.0f, 5.0f, 0.0f, 0.0f)
				[
					SAssignNew(DataScrollBox, SScrollBox)
					//.IsEnabled(this, &SProceduralDungeonEdModeWidget::MatchingDataLevel)
				]
			]
		]
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SVerticalBox)
			.Visibility(this, &SProceduralDungeonEdModeWidget::ShowDataDetails)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Utilities")))
				.Font(SubTitleFont)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Update Selected Volumes")))
					.IsEnabled_Lambda([this]() { return SelectedVolumeCount > 0; })
					.OnClicked(this, &SProceduralDungeonEdModeWidget::UpdateSelectedVolumes)
					.ToolTipText(FText::FromString(TEXT("Selected volumes will be positioned and sized on the room's bounds (defined in data asset).\nAn optional margin can be set.")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(20.0f, 0.0f, 0.0f, 0.0f)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Margins")))
					.ToolTipText(FText::FromString(TEXT("The amount (in Unreal Unit) to extend the volumes on each side of the room bounds (can be negative).")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.0f, 0.0f, 5.0f, 0.0f)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("X")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.XAxis.X)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.XAxis.X = Value; })
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.XAxis.Y)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.XAxis.Y = Value; })
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.0f, 0.0f, 5.0f, 0.0f)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Y")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.YAxis.X)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.YAxis.X = Value; })
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.YAxis.Y)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.YAxis.Y = Value; })
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.0f, 0.0f, 5.0f, 0.0f)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Z")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.ZAxis.X)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.ZAxis.X = Value; })
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SSpinBox<float>)
					.Value(VolumeMargins.ZAxis.Y)
					.OnValueChanged_Lambda([this](float Value) {VolumeMargins.ZAxis.Y = Value; })
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Remove All Invalid Doors")))
					//.IsEnabled_Lambda([this]() { return SelectedVolumeCount > 0; })
					.OnClicked(this, &SProceduralDungeonEdModeWidget::RemoveInvalidDoors)
					.ToolTipText(FText::FromString(TEXT("All invalid doors (drawn in orange) will be removed.")))
				]
			]
		]
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.Visibility(this, &SProceduralDungeonEdModeWidget::ShowNote)
			.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
			.Padding(0.0f)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Reparent Level Blueprint")))
				.OnClicked(this, &SProceduralDungeonEdModeWidget::ReparentLevelActor)
				.ButtonColorAndOpacity(this, &SProceduralDungeonEdModeWidget::GetReparentButtonColor)
				.HAlign(EHorizontalAlignment::HAlign_Center)
			]
		]
	];

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// RoomData details view
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	DataContentWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DataContentWidget->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& Event) { UpdateErrorText(); });
	DataScrollBox->AddSlot()
	[
		DataContentWidget.ToSharedRef()
	];

	OnLevelChanged();

	RegisterSelectionDelegate(true);
	OnSelectedActorsChanged(nullptr);
}

SProceduralDungeonEdModeWidget::~SProceduralDungeonEdModeWidget()
{
	RegisterSelectionDelegate(false);
	ResetCachedData();
	ResetCachedLevel();
}

void SProceduralDungeonEdModeWidget::OnLevelChanged()
{
	FProceduralDungeonEdMode* EdMode = GetEditorMode();
	ResetCachedLevel();
	if (!IsValidRoomLevel(EdMode, &CachedLevel))
	{
		UpdateErrorText();
		return;
	}

	DungeonEd_LogInfo("Slate Editor Level: %s", *GetNameSafe(CachedLevel.Get()));

	LevelDelegateHandle = CachedLevel->OnPropertiesChanged.AddLambda([this](ARoomLevel* RoomLevel) { OnDataAssetChanged(); });

	// RoomLevel Data property
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TSharedPtr<ISinglePropertyView> SinglePropView = PropertyEditorModule.CreateSingleProperty(CachedLevel.Get(), "Data", {});
	LevelPropertyContainer->SetContent(SinglePropView.ToSharedRef());

	OnDataAssetChanged();
}

void SProceduralDungeonEdModeWidget::OnDataAssetChanged()
{
	auto EdMode = GetEditorMode();
	ResetCachedData();
	if (IsValidRoomData(EdMode, &CachedData))
	{
		DataContentWidget->SetObject(CachedData.Get());
		if (CachedData->Level.IsNull())
		{
			CachedData->Modify();
			CachedData->Level = EdMode->GetWorld();
			DungeonEd_LogInfo("Room Data's Level asset filled with current editor's Level.");
		}
		DataDelegateHandle = CachedData->OnPropertiesChanged.AddLambda([this](URoomData* Data) { UpdateErrorText(); });
	}

	UpdateErrorText();

	EdMode->SetDefaultTool();

	FProceduralDungeonEditorTool* ActiveTool = EdMode->GetActiveTool();
	if (ActiveTool)
		ActiveTool->OnDataChanged(CachedData.Get());
}

FReply SProceduralDungeonEdModeWidget::ReparentLevelActor()
{
	auto EdMode = GetEditorMode();
	auto World = EdMode->GetWorld();
	ULevelScriptBlueprint* LevelBlueprint = World->PersistentLevel->GetLevelScriptBlueprint();
	if (!IsValid(LevelBlueprint))
	{
		DungeonEd_LogError("ERROR: Can't Reparent Level Blueprint for an unknown reason.");
		return FReply::Unhandled();
	}

	LevelBlueprint->ParentClass = ARoomLevel::StaticClass();
	FKismetEditorUtilities::CompileBlueprint(LevelBlueprint);

	DungeonEd_LogInfo("Level Blueprint '%s' successfully reparented!", *LevelBlueprint->GetName());

	EdMode->UpdateLevelBlueprint();

	return FReply::Unhandled();
}

FReply SProceduralDungeonEdModeWidget::EditData()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!IsValid(AssetEditorSubsystem))
		return FReply::Unhandled();

	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(nullptr, &Data))
		return FReply::Unhandled();

	AssetEditorSubsystem->OpenEditorForAsset(Data.Get());
	return FReply::Handled();
}

FReply SProceduralDungeonEdModeWidget::SaveData()
{
	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(nullptr, &Data))
		return FReply::Unhandled();

	auto Result = FEditorFileUtils::PromptForCheckoutAndSave({Data->GetPackage()}, /*bCheckDirty = */true, /*bPromptToSave = */false);
	if (Result == FEditorFileUtils::EPromptReturnCode::PR_Success)
		DungeonEd_LogInfo("Successfully Saved Data Asset: '%s'", *GetNameSafe(Data.Get()));
	return FReply::Handled();
}

FReply SProceduralDungeonEdModeWidget::UpdateSelectedVolumes()
{
	DungeonEd_LogInfo("Update Selected Volumes.");

	auto EdMode = GetEditorMode();
	TWeakObjectPtr<ARoomLevel> Level;
	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(EdMode, &Data, &Level))
	{
		DungeonEd_LogError("Can't update selected volumes: RoomData is not valid.");
		return FReply::Unhandled();
	}

	FBoxCenterAndExtent RoomBounds = Data->GetBounds();
	RoomBounds = VolumeMargins.Apply(RoomBounds);

	GEditor->BeginTransaction(FText::FromString(TEXT("Update Selected Volumes")));
	for (auto It = GEditor->GetSelectedActorIterator(); It; ++It)
	{
		AVolume* Volume = Cast<AVolume>(*It);
		if (!IsValid(Volume))
			continue;

		UCubeBuilder* CubeBrush = Cast<UCubeBuilder>(Volume->BrushBuilder);
		if (!IsValid(CubeBrush))
		{
			DungeonEd_LogWarning("Volume's brush is not a cube. Ignoring this volume.");
			continue;
		}

		DungeonEd_LogInfo("Updating volume: '%s'", *Volume->GetName());

		Volume->Modify();
		Volume->SetActorLocationAndRotation(RoomBounds.Center, FQuat::Identity);

		CubeBrush->Modify();
		CubeBrush->X = 2.0f * RoomBounds.Extent.X;
		CubeBrush->Y = 2.0f * RoomBounds.Extent.Y;
		CubeBrush->Z = 2.0f * RoomBounds.Extent.Z;

		// Rebuild volume after changing its builder values
		CubeBrush->Build(Volume->GetWorld(), Volume);
	}
	GEditor->EndTransaction();

	return FReply::Handled();
}

FReply SProceduralDungeonEdModeWidget::RemoveInvalidDoors()
{
	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(nullptr, &Data))
		return FReply::Unhandled();

	GEditor->BeginTransaction(FText::FromString(TEXT("Remove Invalid Doors")));
	Data->Modify();
	for (int i = Data->Doors.Num() - 1; i >= 0; --i)
	{
		if (!Data->IsDoorValid(i) || Data->IsDoorDuplicate(i))
			Data->Doors.RemoveAt(i);
	}
	GEditor->EndTransaction();

	return FReply::Handled();
}

FSlateColor SProceduralDungeonEdModeWidget::GetSaveButtonColor() const
{
	const FLinearColor& Default = FLinearColor::White;
	const FLinearColor& Highlight = FLinearColor::Green;
	return IsDataDirty() ? GetHighlightButtonColor(Highlight, Default) : Default;
}

FSlateColor SProceduralDungeonEdModeWidget::GetReparentButtonColor() const
{
	return GetHighlightButtonColor(FLinearColor::Green);
}

void SProceduralDungeonEdModeWidget::UpdateErrorText()
{
	auto EdMode = GetEditorMode();
	checkf(EdMode, TEXT("EdMode is Invalid in UpdateErrorText"));
	if (!IsValidRoomLevel(EdMode))
		Error->SetError(TEXT("Persistent Level is not a Room Level."));
	else if (!IsValidRoomData(EdMode))
		Error->SetError(TEXT("Room Level has no Room Data set."));
	else if (!MatchingDataLevel(EdMode))
		Error->SetError(TEXT("Level's Data and Data's Level do not match."));
	else
		Error->SetError(FText::GetEmpty());
}

void SProceduralDungeonEdModeWidget::ResetCachedData()
{
	if (!CachedData.IsValid())
		return;
	CachedData->OnPropertiesChanged.Remove(DataDelegateHandle);
	DataDelegateHandle.Reset();
	CachedData.Reset();
}

void SProceduralDungeonEdModeWidget::ResetCachedLevel()
{
	if (!CachedLevel.IsValid())
		return;
	CachedLevel->OnPropertiesChanged.Remove(LevelDelegateHandle);
	LevelDelegateHandle.Reset();
	CachedLevel.Reset();
}

FProceduralDungeonEdMode* SProceduralDungeonEdModeWidget::GetEditorMode() const
{
	checkf(ParentToolkit.IsValid(), TEXT("ParentToolkit is invalid. This should never happen. There is a leakage somewhere."));
	return ParentToolkit.Pin()->GetDungeonEditorMode();
}

void SProceduralDungeonEdModeWidget::RegisterSelectionDelegate(bool Register)
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	checkf(IsValid(SelectedActors), TEXT("Editor Actor Selection is not valid!"));

	if (Register)
	{
		if (SelectionDelegateHandle.IsValid())
		{
			DungeonEd_LogWarning("Can't register SelectionChanged callback: callback is already registered.");
		}
		else
		{
			DungeonEd_LogInfo("Register SelectionChanged callback.");
			SelectionDelegateHandle = SelectedActors->SelectionChangedEvent.AddRaw(this, &SProceduralDungeonEdModeWidget::OnSelectedActorsChanged);
		}
	}
	else
	{
		if (SelectionDelegateHandle.IsValid())
		{
			DungeonEd_LogInfo("Unregister SelectionChanged callback.");
			SelectedActors->SelectionChangedEvent.Remove(SelectionDelegateHandle);
			SelectionDelegateHandle.Reset();
		}
		else
		{
			DungeonEd_LogWarning("Can't unregister SelectionChanged callback: callback is not registered.");
		}
	}
}

void SProceduralDungeonEdModeWidget::OnSelectedActorsChanged(UObject* NewSelectedObject)
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	SelectedVolumeCount = SelectedActors->CountSelections<AVolume>();
}

bool SProceduralDungeonEdModeWidget::IsValidRoomLevel(FProceduralDungeonEdMode* EdMode, TWeakObjectPtr<ARoomLevel>* OutLevel) const
{
	if (!EdMode)
		EdMode = GetEditorMode();

	auto Level = EdMode->GetLevel();
	if (OutLevel)
		*OutLevel = Level;

	return Level.IsValid();
}

bool SProceduralDungeonEdModeWidget::IsValidRoomData(FProceduralDungeonEdMode* EdMode, TWeakObjectPtr<URoomData>* OutData, TWeakObjectPtr<ARoomLevel>* OutLevel) const
{
	if (!EdMode)
		EdMode = GetEditorMode();

	TWeakObjectPtr<ARoomLevel> Level;
	if (!IsValidRoomLevel(EdMode, &Level))
		return false;

	if (OutLevel)
		*OutLevel = Level;

	if (OutData)
		*OutData = Level->Data;

	return IsValid(Level->Data);
}

bool SProceduralDungeonEdModeWidget::MatchingDataLevel(FProceduralDungeonEdMode* EdMode) const
{
	if (!EdMode)
		EdMode = GetEditorMode();

	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(EdMode, &Data))
		return false;

	return Data->Level.GetUniqueID() == EdMode->GetWorld()->GetPathName();
}

bool SProceduralDungeonEdModeWidget::IsDataDirty(FProceduralDungeonEdMode* EdMode) const
{
	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(EdMode, &Data))
		return false;

	return Data->GetPackage()->IsDirty();
}

EVisibility SProceduralDungeonEdModeWidget::ShowDetails() const
{
	return IsValidRoomLevel() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SProceduralDungeonEdModeWidget::ShowDataDetails() const
{
	return IsValidRoomData() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SProceduralDungeonEdModeWidget::ShowNote() const
{
	return IsValidRoomLevel() ? EVisibility::Collapsed : EVisibility::Visible;
}

FText SProceduralDungeonEdModeWidget::GetDataAssetName() const
{
	auto EdMode = GetEditorMode();
	TWeakObjectPtr<URoomData> Data;
	if (!IsValidRoomData(EdMode, &Data))
		return FText::GetEmpty();

	FString Dirty = IsDataDirty(EdMode) ? "*" : "";
	return FText::FromString(GetNameSafe(Data.Get()) + Dirty);
}

FLinearColor SProceduralDungeonEdModeWidget::GetHighlightButtonColor(const FLinearColor& HighlightColor, const FLinearColor& NormalColor, float Speed)
{
	uint32 ticks = FDateTime::Now().GetTicks(); // needs this line to avoid compiler optimization that prevent getting Now() each frame.
	float seconds = static_cast<float>(ticks) / ETimespan::TicksPerSecond;
	float t = FMath::Clamp(FMath::Cos(Speed * seconds), 0.0f, 1.0f);
	return FMath::Lerp(NormalColor, HighlightColor, t);
}
