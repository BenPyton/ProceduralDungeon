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

#include "ProceduralDungeonEdMode.h"
#include "ProceduralDungeonEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "ProceduralDungeonEdModeToolkit.h"
#include "ProceduralDungeonEdLog.h"
#include "Room.h"
#include "RoomLevel.h"
#include "RoomData.h"

IMPLEMENT_HIT_PROXY(HRoomPointProxy, HHitProxy);

FVector ToWorldLocation(FIntVector RoomPoint)
{
    return URoom::Unit() * (FVector(RoomPoint) - FVector(0.5f, 0.5f, 0.0f));
}

FIntVector ToRoomLocation(FVector WorldPoint)
{
    const FVector Unit = URoom::Unit();
    const int X = FMath::RoundToInt(0.5f + (WorldPoint.X) / Unit.X);
    const int Y = FMath::RoundToInt(0.5f + (WorldPoint.Y) / Unit.Y);
    const int Z = FMath::RoundToInt((WorldPoint.Z) / Unit.Z);
    return FIntVector(X, Y, Z);
}

FIntVector ToRoomVector(FVector WorldVector)
{
    const FVector Unit = URoom::Unit();
    const int X = FMath::RoundToInt(WorldVector.X / Unit.X);
    const int Y = FMath::RoundToInt(WorldVector.Y / Unit.Y);
    const int Z = FMath::RoundToInt(WorldVector.Z / Unit.Z);
    return FIntVector(X, Y, Z);
}

FVector SnapPoint(FVector Point)
{
    return ToWorldLocation(ToRoomLocation(Point));
}

void FRoomPoint::AddLinkedPoint(FRoomPoint& Point, EAxisList::Type Axis)
{
    LinkedPoints.Add({ &Point, Axis });
    Point.LinkedPoints.Add({ this, Axis });
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

const FEditorModeID FProceduralDungeonEdMode::EM_ProceduralDungeon(TEXT("EM_ProceduralDungeon"));

void FProceduralDungeonEdMode::Enter()
{
    FEdMode::Enter();

    // Get Current Level Actor
    FWorldContext WorldContext = GEditor->GetEditorWorldContext();
    World = WorldContext.World();
    DungeonEd_LogInfo("World Context: %s", *GetNameSafe(World.Get()));

    Level = Cast<ARoomLevel>(World->GetLevelScriptActor());
    DungeonEd_LogInfo("Room Level: %s", *GetNameSafe(Level.Get()));

    if (Level.IsValid() && IsValid(Level.Get()->Data))
    {
        FIntVector P1 = Level.Get()->Data->FirstPoint;
        FIntVector P2 = Level.Get()->Data->SecondPoint;

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

        Points[0].SetLocation(ToWorldLocation(P1));
        Points[1].SetLocation(ToWorldLocation(P2));
    }

    if (!Toolkit.IsValid())
    {
        Toolkit = MakeShareable(new FProceduralDungeonEdModeToolkit);
        Toolkit->Init(Owner->GetToolkitHost());
    }
}

void FProceduralDungeonEdMode::Exit()
{
    FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
    Toolkit.Reset();

    Level.Reset();

    FEdMode::Exit();
}

void FProceduralDungeonEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
    const FColor NormalColor(200, 200, 200);
    const FColor SelectedColor(255, 128, 0);

    if (Level.IsValid())
    {
        const URoomData* Data = Level.Get()->Data;
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

    FEdMode::Render(View, Viewport, PDI);
}

bool FProceduralDungeonEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    bool bIsHandled = false;

    if (HitProxy)
    {
        if (HitProxy->IsA(HRoomPointProxy::StaticGetType()))
        {
            bIsHandled = true;
            const HRoomPointProxy* RoomProxy = (HRoomPointProxy*)HitProxy;
            if (RoomProxy->Index >= 0 && RoomProxy->Index < Points.Num())
            {
                DungeonEd_LogInfo("Selected Point: %d", RoomProxy->Index);
                SelectedPoint = RoomProxy->Index;
                DragPoint = Points[SelectedPoint].GetLocation();
            }
        }
    }
    else if(SelectedPoint >= 0)
    {
        SelectedPoint = -1;
    }

    return bIsHandled;
}

bool FProceduralDungeonEdMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
    if (InViewportClient->GetCurrentWidgetAxis() == EAxisList::None)
        return false;

    if (HasValidSelection())
    {
        if (!InDrag.IsNearlyZero())
        {
            DragPoint += InDrag;
            FVector OldPoint = Points[SelectedPoint].GetLocation();
            Points[SelectedPoint].SetLocation(SnapPoint(DragPoint));
            if(OldPoint != Points[SelectedPoint].GetLocation())
                UpdateDataAsset();
        }
        return true;
    }

    return false;
}

bool FProceduralDungeonEdMode::ShowModeWidgets() const
{
    return true;
}

bool FProceduralDungeonEdMode::ShouldDrawWidget() const
{
    return true;
}

bool FProceduralDungeonEdMode::UsesTransformWidget() const
{
    return true;
}

bool FProceduralDungeonEdMode::UsesTransformWidget(FWidget::EWidgetMode CheckMode) const
{
    return CheckMode == FWidget::EWidgetMode::WM_Translate;
}

FVector FProceduralDungeonEdMode::GetWidgetLocation() const
{
    if (HasValidSelection())
        return DragPoint;
    return FEdMode::GetWidgetLocation();
}

bool FProceduralDungeonEdMode::HasValidSelection() const
{
    return SelectedPoint >= 0 && SelectedPoint < Points.Num();
}

void FProceduralDungeonEdMode::UpdateDataAsset() const
{
    if (!Level.IsValid())
        return;

    URoomData* Data = Level.Get()->Data;
    if (!IsValid(Data))
        return;

    Data->Modify();
    Data->FirstPoint = ToRoomLocation(Points[0].GetLocation());
    Data->SecondPoint = ToRoomLocation(Points[1].GetLocation());
}
