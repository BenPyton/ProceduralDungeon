// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralDungeonEditorTool.h"

struct HRoomPointProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	HRoomPointProxy(int32 InIndex)
		: HHitProxy(HPP_UI)
		, Index(InIndex)
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
	TArray<FLink> LinkedPoints {};
};

struct FBoxPoints
{
public:
	FBoxPoints();

	void SetID(uint32 InID) { ID = InID; }
	void SetMin(FVector Value);
	void SetMax(FVector Value);
	uint32 GetID() const { return ID; }
	FVector GetMin() const { return Points[0].GetLocation(); }
	FVector GetMax() const { return Points[1].GetLocation(); }

	FRoomPoint& GetPoint(int32 Index)
	{
		check(Index >= 0 && Index < NbPoints);
		return Points[Index];
	}

	void Draw(FPrimitiveDrawInterface* PDI, int32 SelectedPoint) const;

	static const uint32 NbPoints {8};

private:
	uint32 ID;
	FRoomPoint Points[NbPoints];
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
	virtual void OnDataPropertiesChanged(const URoomData* Data = nullptr) override;

	bool HasValidSelection() const;
	void ResetDragPoint();
	void UpdateDataAsset(int32 BoxIndex) const;

	void SetSelectedPoint(int32 Index);
	void ResetSelectedPoint();

private:
	int32 GetBoxIndexFromPointIndex(int32 PointIndex) const { return PointIndex / FBoxPoints::NbPoints; }
	int32 GetBoxPointIndex(int32 PointIndex) const { return PointIndex % FBoxPoints::NbPoints; }
	int32 GetMaxIndex() const { return Boxes.Num() * FBoxPoints::NbPoints; }
	FRoomPoint& GetSelectedPoint();
	void UpdateBoxes(const URoomData* Data);

private:
	int32 SelectedPoint {-1};
	TArray<FBoxPoints> Boxes;
	FVector DragPoint;
};
