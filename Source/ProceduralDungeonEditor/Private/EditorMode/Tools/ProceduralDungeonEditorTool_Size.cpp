// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
	OnDataChanged();
	ResetSelectedPoint();
}

void FProceduralDungeonEditorTool_Size::ExitTool()
{
	DungeonEd_LogInfo("Exit Size Tool.");
}

void FProceduralDungeonEditorTool_Size::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	const URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return;

	for (const FBoxPoints& Box : Boxes)
	{
		Box.Draw(PDI, SelectedPoint);
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
	checkf(RoomProxy->Index >= 0 && RoomProxy->Index < GetMaxIndex(), TEXT("A room point has been clicked but is out of bounds!"));

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

	const URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return false;

	if (!InDrag.IsNearlyZero())
	{
		DragPoint += InDrag;
		FRoomPoint& Point = GetSelectedPoint();
		FVector OldPoint = Point.GetLocation();
		Point.SetLocation(Dungeon::SnapPoint(DragPoint, Data->GetRoomUnit()));
		if (OldPoint != Point.GetLocation())
			UpdateDataAsset(GetBoxIndexFromPointIndex(SelectedPoint));
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
	// When the room data has changed, reset all
	ResetSelectedPoint();
	ResetDragPoint();

	UpdateBoxes(NewData);
}

void FProceduralDungeonEditorTool_Size::OnDataPropertiesChanged(const URoomData* Data)
{
	UpdateBoxes(Data);
}

bool FProceduralDungeonEditorTool_Size::HasValidSelection() const
{
	return SelectedPoint >= 0 && SelectedPoint < GetMaxIndex();
}

void FProceduralDungeonEditorTool_Size::ResetDragPoint()
{
	if (HasValidSelection())
		DragPoint = GetSelectedPoint().GetLocation();
	else
		DragPoint = FVector::ZeroVector;
}

void FProceduralDungeonEditorTool_Size::UpdateDataAsset(int32 BoxIndex) const
{
	URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return;

	if (BoxIndex >= 0 && BoxIndex < Data->BoundingBoxes.Num())
	{
		Data->Modify();
		const FVector RoomUnit = Data->GetRoomUnit();
		const FBoxPoints& Box = Boxes[BoxIndex];
		FBoxMinAndMax& DataBox = Data->BoundingBoxes[BoxIndex];
		const FIntVector A = Dungeon::ToRoomLocation(Box.GetMin(), RoomUnit);
		const FIntVector B = Dungeon::ToRoomLocation(Box.GetMax(), RoomUnit);
		DataBox.SetMinAndMax(A, B);
		return;
	}
}

void FProceduralDungeonEditorTool_Size::SetSelectedPoint(int32 Index)
{
	if (Index < 0 || Index >= GetMaxIndex())
		Index = -1;

	DungeonEd_LogInfo("Selected Point: %d", Index);
	SelectedPoint = Index;
	ResetDragPoint();
}

void FProceduralDungeonEditorTool_Size::ResetSelectedPoint()
{
	SetSelectedPoint(-1);
}

FRoomPoint& FProceduralDungeonEditorTool_Size::GetSelectedPoint()
{
	checkf(HasValidSelection(), TEXT("Trying to get selected point but selection is invalid!"));
	const int32 BoxIndex = GetBoxIndexFromPointIndex(SelectedPoint);
	const int32 PointIndex = GetBoxPointIndex(SelectedPoint);
	checkf(BoxIndex >= 0 && BoxIndex < Boxes.Num(), TEXT("Selected point's box index is out of bounds!"));
	checkf(PointIndex >= 0 && PointIndex < FBoxPoints::NbPoints, TEXT("Selected point's point index is out of bounds!"));
	return Boxes[BoxIndex].GetPoint(PointIndex);
}

void FProceduralDungeonEditorTool_Size::UpdateBoxes(const URoomData* Data)
{
	if (!IsValid(Data))
		return;

	const int32 NbBoxes = Data->BoundingBoxes.Num();
	if (Boxes.Num() != NbBoxes)
	{
		Boxes.SetNum(NbBoxes);
	}

	const FVector RoomUnit = Data->GetRoomUnit();
	for (int32 i = 0; i < NbBoxes; ++i)
	{
		const FBoxMinAndMax& Box = Data->BoundingBoxes[i];
		Boxes[i].SetID(i);
		Boxes[i].SetMin(Dungeon::ToWorldLocation(Box.GetMin(), RoomUnit));
		Boxes[i].SetMax(Dungeon::ToWorldLocation(Box.GetMax(), RoomUnit));
	}
}

FBoxPoints::FBoxPoints()
{
	//        6    1
	//      2    7
	//        4    5
	//      0    3

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
}

void FBoxPoints::SetMin(FVector Value)
{
	Points[0].SetLocation(Value);
}

void FBoxPoints::SetMax(FVector Value)
{
	Points[1].SetLocation(Value);
}

void FBoxPoints::Draw(FPrimitiveDrawInterface* PDI, int32 SelectedPoint) const
{
	static const FColor NormalColor(200, 200, 200);
	static const FColor SelectedColor(255, 128, 0);

	const int32 LocalSelectedIndex = SelectedPoint - (ID * NbPoints);
	for (int i = 0; i < NbPoints; ++i)
	{
		const bool bSelected = (LocalSelectedIndex == i);
		const FColor& Color = bSelected ? SelectedColor : NormalColor;

		PDI->SetHitProxy(new HRoomPointProxy(ID * NbPoints + i));
		PDI->DrawPoint(Points[i].GetLocation(), Color, 10.0f, SDPG_Foreground);
		PDI->SetHitProxy(nullptr);
	}
}
