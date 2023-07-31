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

#include "ProceduralDungeonEditorTool.h"

class FProceduralDungeonEditorTool_Door : public FProceduralDungeonEditorTool
{
public:
	FProceduralDungeonEditorTool_Door(FProceduralDungeonEdMode* InEdMode)
		: FProceduralDungeonEditorTool(InEdMode)
	{
	}

	virtual const TCHAR* GetToolName() override { return TEXT("Tool_Door"); }
	virtual FText GetDisplayName() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Door", "Door"); }
	virtual FText GetDisplayMessage() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Door_Tooltip", ""); }

	/** FProceduralDungeonEditorTool interface */
	virtual void EnterTool() override;
	virtual void ExitTool() override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY) override;

	virtual void OnLevelChanged(const class ARoomLevel* NewLevel) override;
	virtual void OnDataChanged(const URoomData* NewData = nullptr) override;

protected:
	void UpdateRoomBox();
	void DestroyRoomBox();
	bool RoomTraceFromMouse(FHitResult& OutHit, FEditorViewportClient* ViewportClient) const;
	bool RoomTrace(FHitResult& OutHit, const FVector& RayOrigin, const FVector& RayEnd) const;
	bool GetRoomCellFromHit(const FHitResult& Hit, FIntVector& OutCell, EDoorDirection& OutDirection) const;

private:
	TWeakObjectPtr<class ARoomLevel> CachedLevel = nullptr;
	TWeakObjectPtr<class UBoxComponent> RoomBox = nullptr;
	FDoorDef DoorPreview;
	bool ShowDoorPreview {false};
};
