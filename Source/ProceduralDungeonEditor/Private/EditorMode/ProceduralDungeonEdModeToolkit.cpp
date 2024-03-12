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

#include "ProceduralDungeonEdModeToolkit.h"
#include "ProceduralDungeonEdMode.h"
#include "SProceduralDungeonEdModeWidget.h"
#include "EditorModeManager.h"
#include "ISinglePropertyView.h"
#include "ProceduralDungeonEditorCommands.h"
#include "ProceduralDungeonEditorObject.h"
#include "ProceduralDungeonEdLog.h"
#include "Tools/ProceduralDungeonEditorTool.h"

#define LOCTEXT_NAMESPACE "ProceduralDungeonEditor"

void FProceduralDungeonEdModeToolkit::Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost)
{
	auto NameToCommandMap = FProceduralDungeonEditorCommands::Get().NameToCommandMap;

	TSharedRef<FUICommandList> CommandList = GetToolkitCommands();
	CommandList->MapAction(NameToCommandMap.FindChecked("Tool_Size"), FUIAction(
		FExecuteAction::CreateSP(this, &FProceduralDungeonEdModeToolkit::OnChangeTool, FName("Tool_Size")),
		FCanExecuteAction::CreateSP(this, &FProceduralDungeonEdModeToolkit::IsToolEnabled, FName("Tool_Size")),
		FIsActionChecked::CreateSP(this, &FProceduralDungeonEdModeToolkit::IsToolActive, FName("Tool_Size")))
	);

	CommandList->MapAction(NameToCommandMap.FindChecked("Tool_Door"), FUIAction(
		FExecuteAction::CreateSP(this, &FProceduralDungeonEdModeToolkit::OnChangeTool, FName("Tool_Door")),
		FCanExecuteAction::CreateSP(this, &FProceduralDungeonEdModeToolkit::IsToolEnabled, FName("Tool_Door")),
		FIsActionChecked::CreateSP(this, &FProceduralDungeonEdModeToolkit::IsToolActive, FName("Tool_Door")))
	);

	SAssignNew(EdModeWidget, SProceduralDungeonEdModeWidget, SharedThis(this));
	FModeToolkit::Init(InitToolkitHost);
}

void FProceduralDungeonEdModeToolkit::GetToolPaletteNames(TArray<FName>& InPaletteName) const
{
	InPaletteName.Add(FName("DefaultPalette"));
}

FText FProceduralDungeonEdModeToolkit::GetToolPaletteDisplayName(FName PaletteName) const
{
	if (PaletteName == FName("DefaultPalette"))
	{
		return LOCTEXT("Mode.Default", "Default");
	}
	return FText();
}

void FProceduralDungeonEdModeToolkit::BuildToolPalette(FName Palette, FToolBarBuilder& ToolbarBuilder)
{
	auto CommandList = FProceduralDungeonEditorCommands::Get();

	// DoorType property from Settings
	UProceduralDungeonEditorObject* EditorSettings = GetDungeonEditorMode()->Settings;
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams Params;
#if !COMPATIBILITY
	Params.bHideAssetThumbnail = true;
#endif
	Params.NamePlacement = EPropertyNamePlacement::Hidden;
	TSharedPtr<ISinglePropertyView> SinglePropView = PropertyEditorModule.CreateSingleProperty(EditorSettings, "DoorType", Params);

	TSharedPtr<SWidget> Widget =
		SNew(SHorizontalBox)
		.IsEnabled(this, &FProceduralDungeonEdModeToolkit::IsDoorTypeEnabled)
		+ SHorizontalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Center)
		.AutoWidth()
		[
			SinglePropView.ToSharedRef()
		];

	ToolbarBuilder.BeginSection("Default");
	ToolbarBuilder.AddToolBarButton(CommandList.SizeTool);
	ToolbarBuilder.AddSeparator();
	ToolbarBuilder.AddToolBarButton(CommandList.DoorTool);
	ToolbarBuilder.AddWidget(Widget.ToSharedRef());
	ToolbarBuilder.EndSection();
}

FEdMode* FProceduralDungeonEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FProceduralDungeonEdMode::EM_ProceduralDungeon);
}

FProceduralDungeonEdMode* FProceduralDungeonEdModeToolkit::GetDungeonEditorMode() const
{
	return (FProceduralDungeonEdMode*)GetEditorMode();
}

TSharedPtr<SWidget> FProceduralDungeonEdModeToolkit::GetInlineContent() const
{
	return EdModeWidget;
}

void FProceduralDungeonEdModeToolkit::OnChangeTool(FName ToolName) const
{
	FProceduralDungeonEdMode* EdMode = GetDungeonEditorMode();
	if (!EdMode)
	{
		DungeonEd_LogError("Editor Mode is invalid.");
		return;
	}

	DungeonEd_LogInfo("Change Tool to '%s'", *ToolName.ToString());
	EdMode->SetActiveTool(ToolName);
}

bool FProceduralDungeonEdModeToolkit::IsToolEnabled(FName ToolName) const
{
	FProceduralDungeonEdMode* EdMode = GetDungeonEditorMode();
	return EdMode && EdMode->IsToolEnabled(ToolName);
}

bool FProceduralDungeonEdModeToolkit::IsToolActive(FName ToolName) const
{
	FProceduralDungeonEdMode* EdMode = GetDungeonEditorMode();
	if (EdMode)
	{
		FProceduralDungeonEditorTool* Tool = nullptr;
		if (EdMode->GetTool(ToolName, Tool))
			return EdMode->GetActiveTool() == Tool;
	}
	return false;
}

bool FProceduralDungeonEdModeToolkit::IsDoorTypeEnabled() const
{
	return IsToolEnabled("Tool_Door");
}

void FProceduralDungeonEdModeToolkit::OnLevelChanged()
{
	EdModeWidget->OnLevelChanged();
}

#undef LOCTEXT_NAMESPACE
