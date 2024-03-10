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
#include "ProceduralDungeonEditorTool.h"

struct HRoomPointProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	HRoomPointProxy(int32 InIndex)
		: HHitProxy(HPP_UI), Index(InIndex)
	{
	}

	int32 Index {-1};
};

struct FRoomPoint
{
	struct FLink
	{
		FRoomPoint* Point;
		EAxisList::Type Axis;
	};

	void AddLinkedPoint(FRoomPoint& Point, EAxisList::Type Axis);
	FVector GetLocation() const { return Location; }
	void SetLocation(FVector InLocation);
	FVector* operator->() { return &Location; }

private:
	void SetDirty();
	void UpdateWithPropagation();
	void UpdateLinkedPoints(TQueue<FRoomPoint*>& PendingNodes);
	void UpdateFrom(FRoomPoint& From, EAxisList::Type Axis);

private:
	EAxisList::Type bDirty {EAxisList::None};
	FVector Location {0};
	TArray<FLink> LinkedPoints{};
};

class FProceduralDungeonEditorTool_Size : public FProceduralDungeonEditorTool
{
public:
	FProceduralDungeonEditorTool_Size(FProceduralDungeonEdMode* InEdMode)
		: FProceduralDungeonEditorTool(InEdMode)
	{
	}

	virtual const TCHAR* GetToolName() override { return TEXT("Tool_Size"); }
	virtual FText GetDisplayName() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Size", "Size"); }
	virtual FText GetDisplayMessage() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Size_Tooltip", ""); }

	virtual void EnterTool() override;
	virtual void ExitTool() override;

	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent) override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual bool UsesTransformWidget() const override;
	virtual bool UsesTransformWidget(WidgetMode CheckMode) const override;
	virtual FVector GetWidgetLocation() const override;

	/** FEditorUndoClient interface */
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	virtual void OnDataChanged(const URoomData* NewData = nullptr) override;

	bool HasValidSelection() const;
	void ResetDragPoint();
	void UpdateDataAsset() const;

	void SetSelectedPoint(int32 Index);
	void ResetSelectedPoint();

private:
	int32 SelectedPoint {-1};
	TArray<FRoomPoint> Points;
	FVector DragPoint;
};
