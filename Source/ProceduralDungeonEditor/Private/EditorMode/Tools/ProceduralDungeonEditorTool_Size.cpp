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

#include "ProceduralDungeonEditorTool_Size.h"
#include "EditorMode/ProceduralDungeonEdMode.h"
#include "EditorModeManager.h"
#include "Engine/Selection.h"
#include "ProceduralDungeonEdLog.h"
#include "ProceduralDungeonUtils.h"
#include "Room.h"
#include "RoomLevel.h"
#include "RoomData.h"

IMPLEMENT_HIT_PROXY(HRoomPointProxy, HHitProxy);

void FRoomPoint::AddLinkedPoint(FRoomPoint& Point, EAxisList::Type Axis)
{
	LinkedPoints.Add({&Point, Axis});
	Point.LinkedPoints.Add({this, Axis});
}

void FRoomPoint::SetLocation(FVector InLocation)
{
	SetDirty();
	Location = InLocation;
	UpdateWithPropagation();
}

void FRoomPoint::SetDirty()
{
	if (bDirty)
		return;

	bDirty = EAxisList::XYZ;
	for (FLink& Link : LinkedPoints)
	{
		check(Link.Point);
		Link.Point->SetDirty();
	}
}

void FRoomPoint::UpdateWithPropagation()
{
	TQueue<FRoomPoint*> PendingNodes;
	PendingNodes.Enqueue(this);
	FRoomPoint* Node = nullptr;
	while (PendingNodes.Dequeue(Node))
	{
		Node->UpdateLinkedPoints(PendingNodes);
	}
}

void FRoomPoint::UpdateLinkedPoints(TQueue<FRoomPoint*>& PendingNodes)
{
	for (FLink& Link : LinkedPoints)
	{
		check(Link.Point);
		if (Link.Point->bDirty)
		{
			Link.Point->UpdateFrom(*this, Link.Axis);
			PendingNodes.Enqueue(Link.Point);
		}
	}
}

void FRoomPoint::UpdateFrom(FRoomPoint& From, EAxisList::Type Axis)
{
	if (bDirty & Axis & EAxisList::X)
	{
		bDirty &= ~EAxisList::X;
		From.bDirty &= ~EAxisList::X;
		Location.X = From.Location.X;
	}
	if (bDirty & Axis & EAxisList::Y)
	{
		bDirty &= ~EAxisList::Y;
		From.bDirty &= ~EAxisList::Y;
		Location.Y = From.Location.Y;
	}
	if (bDirty & Axis & EAxisList::Z)
	{
		bDirty &= ~EAxisList::Z;
		From.bDirty &= ~EAxisList::Z;
		Location.Z = From.Location.Z;
	}
}

void FProceduralDungeonEditorTool_Size::EnterTool()
{
	//        6    1
	//      2    7
	//        4    5
	//      0    3

	Points.Empty();
	Points.AddDefaulted(8);

	Points[0].AddLinkedPoint(Points[2], EAxisList::XY);
	Points[0].AddLinkedPoint(Points[3], EAxisList::XZ);
	Points[0].AddLinkedPoint(Points[4], EAxisList::YZ);

	Points[1].AddLinkedPoint(Points[5], EAxisList::XY);
	Points[1].AddLinkedPoint(Points[6], EAxisList::XZ);
	Points[1].AddLinkedPoint(Points[7], EAxisList::YZ);

	Points[2].AddLinkedPoint(Points[6], EAxisList::YZ);
	Points[2].AddLinkedPoint(Points[7], EAxisList::XZ);

	Points[3].AddLinkedPoint(Points[5], EAxisList::YZ);
	Points[3].AddLinkedPoint(Points[7], EAxisList::XY);

	Points[4].AddLinkedPoint(Points[5], EAxisList::XZ);
	Points[4].AddLinkedPoint(Points[6], EAxisList::XY);

	OnDataChanged();
	ResetSelectedPoint();
}

void FProceduralDungeonEditorTool_Size::ExitTool()
{
	DungeonEd_LogInfo("Exit Size Tool.");
}

void FProceduralDungeonEditorTool_Size::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	const FColor NormalColor(200, 200, 200);
	const FColor SelectedColor(255, 128, 0);

	auto Level = EdMode->GetLevel();
	if (Level.IsValid())
	{
		const URoomData* Data = Level->Data;
		if (IsValid(Data))
		{
			for (int i = 0; i < Points.Num(); ++i)
			{
				const bool bSelected = (SelectedPoint == i);
				const FColor& Color = bSelected ? SelectedColor : NormalColor;

				PDI->SetHitProxy(new HRoomPointProxy(i));
				PDI->DrawPoint(Points[i].GetLocation(), Color, 15.0f, SDPG_Foreground);
				PDI->SetHitProxy(nullptr);
			}
		}
	}
}

bool FProceduralDungeonEditorTool_Size::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	ResetSelectedPoint();

	if (!HitProxy)
		return false;

	if (!HitProxy->IsA(HRoomPointProxy::StaticGetType()))
		return false;

	const HRoomPointProxy* RoomProxy = (HRoomPointProxy*)HitProxy;
	checkf(RoomProxy->Index >= 0 && RoomProxy->Index < Points.Num(), TEXT("A room point has been clicked but is out of bounds!"));

	SetSelectedPoint(RoomProxy->Index);
	GEditor->GetSelectedActors()->DeselectAll();

	// Force translate widget when a point is selected.
	FEditorModeTools* ModeTools = EdMode->GetModeManager();
	if (ModeTools)
		ModeTools->SetWidgetMode(WidgetMode::WM_Translate);

	return true;
}

bool FProceduralDungeonEditorTool_Size::InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent)
{
	if (InKey == EKeys::LeftMouseButton && InEvent == IE_Released)
		ResetDragPoint();
	return false;
}

bool FProceduralDungeonEditorTool_Size::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	if (InViewportClient->GetCurrentWidgetAxis() == EAxisList::None)
		return false;

	if (!HasValidSelection())
		return false;

	if (!InDrag.IsNearlyZero())
	{
		DragPoint += InDrag;
		FVector OldPoint = Points[SelectedPoint].GetLocation();
		Points[SelectedPoint].SetLocation(Dungeon::SnapPoint(DragPoint));
		if (OldPoint != Points[SelectedPoint].GetLocation())
			UpdateDataAsset();
	}
	return true;
}

bool FProceduralDungeonEditorTool_Size::UsesTransformWidget() const
{
	return HasValidSelection() || (GEditor->GetSelectedActorCount() > 0);
}

bool FProceduralDungeonEditorTool_Size::UsesTransformWidget(WidgetMode CheckMode) const
{
	return HasValidSelection() ? (CheckMode == WidgetMode::WM_Translate) : EdMode->FEdMode::UsesTransformWidget(CheckMode);
}

FVector FProceduralDungeonEditorTool_Size::GetWidgetLocation() const
{
	return HasValidSelection() ? DragPoint : EdMode->FEdMode::GetWidgetLocation();
}

void FProceduralDungeonEditorTool_Size::PostUndo(bool bSuccess)
{
	OnDataChanged();
}

void FProceduralDungeonEditorTool_Size::PostRedo(bool bSuccess)
{
	OnDataChanged();
}

void FProceduralDungeonEditorTool_Size::OnDataChanged(const URoomData* NewData)
{
	TWeakObjectPtr<ARoomLevel> Level = EdMode->GetLevel();
	if (!Level.IsValid() || !IsValid(Level->Data))
		return;

	Points[0].SetLocation(Dungeon::ToWorldLocation(Level->Data->FirstPoint));
	Points[1].SetLocation(Dungeon::ToWorldLocation(Level->Data->SecondPoint));

	ResetDragPoint();
}

bool FProceduralDungeonEditorTool_Size::HasValidSelection() const
{
	return SelectedPoint >= 0 && SelectedPoint < Points.Num();
}

void FProceduralDungeonEditorTool_Size::ResetDragPoint()
{
	if (HasValidSelection())
		DragPoint = Points[SelectedPoint].GetLocation();
}

void FProceduralDungeonEditorTool_Size::UpdateDataAsset() const
{
	auto Level = EdMode->GetLevel();
	if (!Level.IsValid())
		return;

	URoomData* Data = Level.Get()->Data;
	if (!IsValid(Data))
		return;

	Data->Modify();
	Data->FirstPoint = Dungeon::ToRoomLocation(Points[0].GetLocation());
	Data->SecondPoint = Dungeon::ToRoomLocation(Points[1].GetLocation());
}

void FProceduralDungeonEditorTool_Size::SetSelectedPoint(int32 Index)
{
	if (Index < 0 || Index >= Points.Num())
		Index = -1;

	DungeonEd_LogInfo("Selected Point: %d", Index);
	SelectedPoint = Index;
	ResetDragPoint();
}

void FProceduralDungeonEditorTool_Size::ResetSelectedPoint()
{
	SetSelectedPoint(-1);
}
