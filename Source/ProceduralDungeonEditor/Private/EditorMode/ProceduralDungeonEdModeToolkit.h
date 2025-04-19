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

#include "Toolkits/BaseToolkit.h"

class FProceduralDungeonEdModeToolkit : public FModeToolkit
{
public:
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<class IToolkitHost>& InitToolkitHost) override;
	virtual void GetToolPaletteNames(TArray<FName>& InPaletteName) const override;
	virtual FText GetToolPaletteDisplayName(FName PaletteName) const override;
	virtual void BuildToolPalette(FName Palette, class FToolBarBuilder& ToolbarBuilder) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override { return FName("ProceduralDungeonEdMode"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("ProceduralDungeonEdModeToolkit", "DisplayName", "ProceduralDungeonEdMode Tool"); }
	virtual FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

	class FProceduralDungeonEdMode* GetDungeonEditorMode() const;

	void OnChangeTool(FName ToolName) const;
	bool IsToolEnabled(FName ToolName) const;
	bool IsToolActive(FName ToolName) const;
	bool IsDoorTypeEnabled() const;
	void OnLevelChanged();

private:
	TSharedPtr<class SProceduralDungeonEdModeWidget> EdModeWidget;
};
