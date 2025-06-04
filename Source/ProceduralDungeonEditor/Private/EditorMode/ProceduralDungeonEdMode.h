// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

	//~ Begin FEdMode Interface
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
	virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;
	//~ End FEdMode Interface

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
	GC_PTR(class UProceduralDungeonEditorObject) Settings { nullptr };

private:
	TArray<TUniquePtr<FProceduralDungeonEditorTool>> Tools;
	FProceduralDungeonEditorTool* ActiveTool = nullptr;
	TWeakObjectPtr<ARoomLevel> CachedLevelInstance = nullptr;
	TWeakObjectPtr<ULevelScriptBlueprint> CachedLevelBlueprint = nullptr;
	FDelegateHandle LevelBlueprintDelegateHandle;
};
