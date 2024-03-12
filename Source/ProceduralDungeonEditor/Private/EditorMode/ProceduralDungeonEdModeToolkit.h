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

#include "Toolkits/BaseToolkit.h"

class FProceduralDungeonEdModeToolkit : public FModeToolkit
{
public:
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost) override;
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
