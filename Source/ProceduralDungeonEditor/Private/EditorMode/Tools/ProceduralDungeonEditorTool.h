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

	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override {}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FProceduralDungeonEditorTool");
	}

	virtual void OnLevelChanged(const class ARoomLevel* NewLevel) {}
	virtual void OnDataChanged(const class URoomData* NewData) {}

protected:
	FProceduralDungeonEdMode* EdMode = nullptr;
};
