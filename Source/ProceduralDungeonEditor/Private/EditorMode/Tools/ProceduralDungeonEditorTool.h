/*
 * Copyright (c) 2023-2025 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "EdMode.h"
#include "EditorUndoClient.h"
#include "ProceduralDungeonEdTypes.h"

class FProceduralDungeonEdMode;

class FProceduralDungeonEditorTool : public FGCObject, public FSelfRegisteringEditorUndoClient
{
public:
	FProceduralDungeonEditorTool(FProceduralDungeonEdMode* InEdMode)
		: EdMode(InEdMode)
	{
	}

	virtual const TCHAR* GetToolName() = 0;
	virtual FText GetDisplayName() = 0;
	virtual FText GetDisplayMessage() = 0;

	virtual void EnterTool() = 0;
	virtual void ExitTool() = 0;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) {}
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) {}
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY) { return false; }
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) { return false; }
	virtual bool InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent) { return false; }
	virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) { return false; }
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) { return false; }
	virtual bool UsesTransformWidget() const { return false; }
	virtual bool UsesTransformWidget(WidgetMode CheckMode) const { return false; }
	virtual FVector GetWidgetLocation() const { return FVector::ZeroVector; }
	virtual bool GetCursor(EMouseCursor::Type& OutCursor) const { return false; }

	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override {}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FProceduralDungeonEditorTool");
	}
	//~ End FGCObject Interface

	virtual void OnLevelChanged(const class ARoomLevel* NewLevel) {}
	virtual void OnDataChanged(const class URoomData* NewData) {}

protected:
	FProceduralDungeonEdMode* EdMode = nullptr;
};
