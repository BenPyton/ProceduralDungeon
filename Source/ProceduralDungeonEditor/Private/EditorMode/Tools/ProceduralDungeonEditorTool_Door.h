// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "ProceduralDungeonEditorTool.h"

class FProceduralDungeonEditorTool_Door : public FProceduralDungeonEditorTool
{
public:
	FProceduralDungeonEditorTool_Door(FProceduralDungeonEdMode* InEdMode)
		: FProceduralDungeonEditorTool(InEdMode)
	{
	}

	//~ Begin FProceduralDungeonEditorTool Interface
	virtual const TCHAR* GetToolName() override { return TEXT("Tool_Door"); }
	virtual FText GetDisplayName() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Door", "Door"); }
	virtual FText GetDisplayMessage() override { return NSLOCTEXT("ProceduralDungeonEditor", "Tool_Door_Tooltip", ""); }

	virtual void EnterTool() override;
	virtual void ExitTool() override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY) override;
	virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;

	virtual void OnLevelChanged(const class ARoomLevel* NewLevel) override;
	virtual void OnDataChanged(const URoomData* NewData = nullptr) override;
	//~ End FProceduralDungeonEditorTool Interface

protected:
	void UpdateRoomBox();
	void DestroyRoomBox();
	bool RoomTraceFromMouse(FHitResult& OutHit, FEditorViewportClient* ViewportClient) const;
	bool RoomTrace(FHitResult& OutHit, const FVector& RayOrigin, const FVector& RayEnd) const;
	bool GetRoomCellFromHit(const FHitResult& Hit, const FVector RoomUnit, FIntVector& OutCell, EDoorDirection& OutDirection) const;

private:
	TWeakObjectPtr<class ARoomLevel> CachedLevel = nullptr;
	TWeakObjectPtr<class UBoxComponent> RoomBox = nullptr;
	FDoorDef DoorPreview;
	bool ShowDoorPreview {false};
};
