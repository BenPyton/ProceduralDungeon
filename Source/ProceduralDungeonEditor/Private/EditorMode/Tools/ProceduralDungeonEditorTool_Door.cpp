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

#include "ProceduralDungeonEditorTool_Door.h"
#include "Components/BoxComponent.h"
#include "EditorMode/ProceduralDungeonEdMode.h"
#include "EditorMode/ProceduralDungeonEditorObject.h"
#include "ProceduralDungeonEdLog.h"
#include "ProceduralDungeonUtils.h"
#include "Room.h"
#include "RoomLevel.h"
#include "RoomData.h"
#include "Door.h"

bool IsDoorValid(const URoomData* Data, const FDoorDef& Door)
{
    check(IsValid(Data));
    for (const FDoorDef& DoorDef : Data->Doors)
    {
        if (DoorDef == Door)
            return false;
    }
    return true;
}

void FProceduralDungeonEditorTool_Door::EnterTool()
{
    DungeonEd_LogInfo("Enter Door Tool.");
    auto Level = EdMode->Level;

    if (!Level.IsValid() || !IsValid(Level->Data))
        return;

    RoomBox = NewObject<UBoxComponent>(Level.Get(), TEXT("Editor Room Collision"), RF_Transient);
    RoomBox->SetupAttachment(Level.Get()->GetRootComponent());
    RoomBox->RegisterComponent();
    RoomBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RoomBox->SetCollisionObjectType(ECollisionChannel::ECC_MAX);

    FBoxCenterAndExtent Box = Level->Data->GetBounds();
    RoomBox->SetRelativeLocation(Box.Center);
    RoomBox->SetBoxExtent(Box.Extent);

    DungeonEd_LogInfo("[Enter] RoomBox: %s", *GetNameSafe(RoomBox.Get()));
}

void FProceduralDungeonEditorTool_Door::ExitTool()
{
    DungeonEd_LogInfo("Exit Door Tool.");
    if(RoomBox.IsValid())
    { 
        RoomBox->DestroyComponent();
        RoomBox.Reset();
    }
}

void FProceduralDungeonEditorTool_Door::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
    FProceduralDungeonEditorTool::Render(View, Viewport, PDI);

    auto Level = EdMode->Level;
    if (!Level.IsValid() || !IsValid(Level->Data))
        return;

    FIntVector Min, Max;
    IntVector::MinMax(Level->Data->FirstPoint, Level->Data->SecondPoint, Min, Max);

    const FColor LineColor(100, 20, 0);

    // Vertical Lines on X
    for (int32 i = Min.X + 1; i < Max.X; ++i)
    {
        FIntVector BottomA(i, Min.Y, Min.Z);
        FIntVector BottomB(i, Max.Y, Min.Z);
        FIntVector TopA(i, Min.Y, Max.Z);
        FIntVector TopB(i, Max.Y, Max.Z);
        PDI->DrawLine(Dungeon::ToWorldLocation(BottomA), Dungeon::ToWorldLocation(TopA), LineColor, SDPG_World);
        PDI->DrawLine(Dungeon::ToWorldLocation(BottomB), Dungeon::ToWorldLocation(TopB), LineColor, SDPG_World);
    }

    // Vertical Lines on Y
    for (int32 i = Min.Y + 1; i < Max.Y; ++i)
    {
        FIntVector BottomA(Min.X, i, Min.Z);
        FIntVector BottomB(Max.X, i, Min.Z);
        FIntVector TopA(Min.X, i, Max.Z);
        FIntVector TopB(Max.X, i, Max.Z);
        PDI->DrawLine(Dungeon::ToWorldLocation(BottomA), Dungeon::ToWorldLocation(TopA), LineColor, SDPG_World);
        PDI->DrawLine(Dungeon::ToWorldLocation(BottomB), Dungeon::ToWorldLocation(TopB), LineColor, SDPG_World);
    }

    // Horizontal Lines on X and Y
    for (int32 i = Min.Z + 1; i < Max.Z; ++i)
    {
        FIntVector A(Min.X, Min.Y, i);
        FIntVector B(Min.X, Max.Y, i);
        FIntVector C(Max.X, Max.Y, i);
        FIntVector D(Max.X, Min.Y, i);
        PDI->DrawLine(Dungeon::ToWorldLocation(A), Dungeon::ToWorldLocation(B), LineColor, SDPG_World);
        PDI->DrawLine(Dungeon::ToWorldLocation(B), Dungeon::ToWorldLocation(C), LineColor, SDPG_World);
        PDI->DrawLine(Dungeon::ToWorldLocation(C), Dungeon::ToWorldLocation(D), LineColor, SDPG_World);
        PDI->DrawLine(Dungeon::ToWorldLocation(D), Dungeon::ToWorldLocation(A), LineColor, SDPG_World);
    }
}

void FProceduralDungeonEditorTool_Door::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FProceduralDungeonEditorTool::Tick(ViewportClient, DeltaTime);

    if (ShowDoorPreview)
    {
        URoomData* Data = EdMode->Level->Data;
        check(IsValid(Data));

        UWorld* World = ViewportClient->GetWorld();
        FColor Color = IsDoorValid(Data, DoorPreview) ? FColor::Green : FColor::Orange;
        FDoorDef::DrawDebug(World, Color, DoorPreview, FTransform::Identity, /*includeOffset = */true);
    }
}

bool FProceduralDungeonEditorTool_Door::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    if (Click.IsAltDown() || Click.IsControlDown() || Click.IsShiftDown())
        return false;

    if (!ShowDoorPreview)
        return false;

    URoomData* Data = EdMode->Level->Data;
    check(IsValid(Data));

    if (Click.GetKey() == EKeys::LeftMouseButton)
    {
        if (IsDoorValid(Data, DoorPreview))
        {
            Data->Modify();
            Data->Doors.Add(DoorPreview);
            return true;
        }
    }

    if (Click.GetKey() == EKeys::RightMouseButton)
    {
        DungeonEd_LogInfo("TODO: Delete existing door");
        return true;
    }

    return false;
}

bool FProceduralDungeonEditorTool_Door::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY)
{
    ShowDoorPreview = false;

    FHitResult Hit;
    if (!RoomTraceFromMouse(Hit, ViewportClient))
        return false;

    FIntVector RoomCell;
    EDoorDirection DoorDirection;
    if (!GetRoomCellFromHit(Hit, RoomCell, DoorDirection))
        return false;

    ShowDoorPreview = true;
    DoorPreview.Position = RoomCell;
    DoorPreview.Direction = DoorDirection;
    DoorPreview.Type = EdMode->Settings->DoorType;

    return false;
}

bool FProceduralDungeonEditorTool_Door::RoomTraceFromMouse(FHitResult& OutHit, FEditorViewportClient* ViewportClient) const
{
    int32 MouseX = ViewportClient->Viewport->GetMouseX();
    int32 MouseY = ViewportClient->Viewport->GetMouseY();

    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext ViewFamily(
        FSceneViewFamilyContext::ConstructionValues(ViewportClient->Viewport, ViewportClient->GetScene(), ViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(ViewportClient->IsRealtime())
    );

    FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
    FViewportCursorLocation MouseViewportRay(View, ViewportClient, MouseX, MouseY);
    FVector MouseViewportRayDirection = MouseViewportRay.GetDirection();

    FVector Start = MouseViewportRay.GetOrigin();
    FVector End = Start + WORLD_MAX * MouseViewportRayDirection;
    if (ViewportClient->IsOrtho())
    {
        Start -= WORLD_MAX * MouseViewportRayDirection;
    }

    return RoomTrace(OutHit, Start, End);
}

bool FProceduralDungeonEditorTool_Door::RoomTrace(FHitResult& OutHit, const FVector& RayOrigin, const FVector& RayEnd) const
{
    if (!RoomBox.IsValid())
        return false;

    return RoomBox->LineTraceComponent(OutHit, RayOrigin, RayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(RoomTrace)));
}

bool FProceduralDungeonEditorTool_Door::GetRoomCellFromHit(const FHitResult& Hit, FIntVector& OutCell, EDoorDirection& OutDirection) const
{
    // Direction is up or down: invalid
    if (FMath::Abs(FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector)) >= 0.5f)
        return false;

    // Determine direction from the hit normal
    float DirX = FVector::DotProduct(Hit.ImpactNormal, FVector::ForwardVector);
    float DirY = FVector::DotProduct(Hit.ImpactNormal, FVector::RightVector);
    if (FMath::Abs(DirX) > FMath::Abs(DirY))
        OutDirection = (DirX > 0) ? EDoorDirection::North : EDoorDirection::South;
    else
        OutDirection = (DirY > 0) ? EDoorDirection::East : EDoorDirection::West;

    // Determine the room cell
    FVector RoomSpacePoint = Hit.ImpactPoint / URoom::Unit();
    RoomSpacePoint -= 0.5f * (ToVector(OutDirection) + FVector::UpVector);
    OutCell = FIntVector(FMath::RoundToInt(RoomSpacePoint.X), FMath::RoundToInt(RoomSpacePoint.Y), FMath::RoundToInt(RoomSpacePoint.Z));
    return true;
}
