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

#include "EdMode.h"
#include "ProceduralDungeonEdTypes.h"
#include "EditorMode/Tools/ProceduralDungeonEditorTool.h"

class ARoomLevel;
class ULevelScriptBlueprint;

class FProceduralDungeonEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_ProceduralDungeon;

	FProceduralDungeonEdMode();

	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
	virtual bool ShowModeWidgets() const override;
	virtual bool ShouldDrawWidget() const override;
	virtual bool UsesTransformWidget() const override;
	virtual bool UsesTransformWidget(WidgetMode CheckMode) const;
	virtual FVector GetWidgetLocation() const override;
	virtual bool AllowWidgetMove() override { return true; }
	virtual bool GetPivotForOrbit(FVector& OutPivot) const override;

	bool GetTool(FName ToolName, FProceduralDungeonEditorTool*& OutTool) const;
	FProceduralDungeonEditorTool* GetActiveTool() const;
	void SetActiveTool(FName ToolName);
	void ResetActiveTool();
	void SetDefaultTool();
	bool IsToolEnabled(FName ToolName) const;

	ULevelScriptBlueprint* GetLevelBlueprint(bool bCreate = false) const;
	TWeakObjectPtr<ARoomLevel> GetLevel() const;
	TWeakObjectPtr<ARoomLevel> GetLevelInstance() const { return CachedLevelInstance; }
	void UpdateLevelBlueprint();

protected:
	void SetActiveTool(FProceduralDungeonEditorTool* NewTool);
	void OnLevelBlueprintCompiled(UBlueprint* Blueprint = nullptr);
	void RegisterLevelCompilationDelegate(bool Register);

public:
	GC_PTR(class UProceduralDungeonEditorObject) Settings {nullptr};

private:
	TArray<TUniquePtr<FProceduralDungeonEditorTool>> Tools;
	FProceduralDungeonEditorTool* ActiveTool = nullptr;
	TWeakObjectPtr<ARoomLevel> CachedLevelInstance = nullptr;
	TWeakObjectPtr<ULevelScriptBlueprint> CachedLevelBlueprint = nullptr;
	FDelegateHandle LevelBlueprintDelegateHandle;
};
