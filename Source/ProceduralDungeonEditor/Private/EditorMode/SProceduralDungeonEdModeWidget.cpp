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

#include "SProceduralDungeonEdModeWidget.h"
#include "EditorModeManager.h"
#include "EditorStyleSet.h"
#include "ISinglePropertyView.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SErrorText.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SOverlay.h"
#include "ProceduralDungeonEdLog.h"
#include "ProceduralDungeonEditor.h"
#include "ProceduralDungeonEdMode.h"
#include "ProceduralDungeonEdModeToolkit.h"
#include "Room.h" // TODO: remove the need to include Room.h when including RoomLevel.h
#include "RoomLevel.h"
#include "RoomData.h"

void SProceduralDungeonEdModeWidget::Construct(const FArguments& InArgs, TSharedRef<FProceduralDungeonEdModeToolkit> InParentToolkit)
{
    ParentToolkit = InParentToolkit;
    Level = InParentToolkit->GetEditorMode()->Level;
    DungeonEd_LogInfo("Slate Editor Level: %s", *GetNameSafe(Level.Get()));

    FText LevelName = FText::FromString(GetNameSafe(InParentToolkit->GetEditorMode()->World.Get()));

    FSlateFontInfo TitleFont = FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle");
    TitleFont.Size = 24;

    FSlateFontInfo SubTitleFont = FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle");
    SubTitleFont.Size = 16;

    TSharedPtr<SScrollBox> DataScrollBox = nullptr;
    TSharedPtr<SBorder> LevelPropertyContainer = nullptr;

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
            .BorderImage(FEditorStyle::GetBrush("DetailsView.CollapsedCategory"))
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
        [
            SNew(STextBlock)
            .Text_Static(&SProceduralDungeonEdModeWidget::GetNoteText)
            .AutoWrapText(true)
            .Visibility(this, &SProceduralDungeonEdModeWidget::ShowNote)
        ]
    ];

    // Check Level script actor validity
    if (!IsValidRoomLevel())
    {
        UpdateErrorText();
        return;
    }

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // RoomLevel Data property
    TSharedPtr<ISinglePropertyView> SinglePropView = PropertyEditorModule.CreateSingleProperty(Level.Get(), "Data", {});
    FSimpleDelegate PropertyChangedDelegate = FSimpleDelegate::CreateSP(this, &SProceduralDungeonEdModeWidget::OnDataAssetChanged);
    SinglePropView->SetOnPropertyValueChanged(PropertyChangedDelegate);
    LevelPropertyContainer->SetContent(SinglePropView.ToSharedRef());

    // Check Data asset validity
    URoomData* Data = Level.Get()->Data;
    if (!IsValid(Data))
    {
        UpdateErrorText();
        return;
    }

    // Update the error if the data asset is edited externally
    Data->OnPropertiesChanged.AddLambda([this](URoomData* Data) { UpdateErrorText(); });

    // RoomData details view
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    DataContentWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DataContentWidget->SetObject(Data);
    DataContentWidget->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& Event) { UpdateErrorText(); });
    DataScrollBox->AddSlot()
    [
        DataContentWidget.ToSharedRef()
    ];
}

void SProceduralDungeonEdModeWidget::OnDataAssetChanged()
{
    if (IsValidRoomData())
        DataContentWidget->SetObject(Level.Get()->Data);

    UpdateErrorText();
}

FReply SProceduralDungeonEdModeWidget::EditData()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    if (!IsValid(AssetEditorSubsystem))
        return FReply::Unhandled();

    AssetEditorSubsystem->OpenEditorForAsset(Level.Get()->Data);
    return FReply::Handled();
}

void SProceduralDungeonEdModeWidget::UpdateErrorText()
{
    DungeonEd_LogInfo("Update Error Text!");

    if (!IsValidRoomLevel())
        Error->SetError(TEXT("Persistent Level is not a Room Level."));
    else if (!IsValidRoomData())
        Error->SetError(TEXT("Room Level has no Room Data set."));
    else if (!MatchingDataLevel())
        Error->SetError(TEXT("Level's Data and Data's Level do not match."));
    else 
        Error->SetError(FText::GetEmpty());
}

bool SProceduralDungeonEdModeWidget::IsValidRoomLevel() const
{
    return Level.IsValid();
}

bool SProceduralDungeonEdModeWidget::IsValidRoomData() const
{
    if (!IsValidRoomLevel())
        return false;
    return IsValid(Level.Get()->Data);
}

bool SProceduralDungeonEdModeWidget::MatchingDataLevel() const
{
    if (!IsValidRoomData())
        return false;
    return Level.Get()->Data->Level.GetUniqueID() == Level.Get()->GetWorld()->GetPathName();
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
    return IsValidRoomLevel() 
        ? FText::FromString(GetNameSafe(Level.Get()->Data))
        : FText::GetEmpty();
}

FText SProceduralDungeonEdModeWidget::GetNoteText()
{
    // TODO: Create a button to automatically reparent the level blueprint to ARoomLevel
    // (see FBlueprintEditor::ReparentBlueprint_NewParentChosen for how it is implemented)
    return FText::FromString(
        TEXT("You have to reparent your level blueprint.")\
        TEXT("\nGo to the level blueprint (Blueprints -> \"Open Level Blueprint\") then reparent the blueprint to \"RoomLevel\".")\
        TEXT("\n(If you want to create a lot of rooms, you can go to \"Project Settings->Engine->General Settings\"")\
        TEXT(" and set the \"Level Script Actor Class\" to \"RoomLevel\" thus you don't have to reparent each level)"));
}
