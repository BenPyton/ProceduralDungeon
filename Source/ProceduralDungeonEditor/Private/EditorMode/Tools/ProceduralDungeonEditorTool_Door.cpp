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

bool AreDoorsOverlapping(const FDoorDef& DoorA, const FDoorDef& DoorB)
{
	// If not in same direction, they will never overlap.
	if (DoorA.Direction != DoorB.Direction)
		return false;

	// If same direction and position, then will always overlap.
	if (DoorA.Position == DoorB.Position)
		return true;

	const FBoxCenterAndExtent DoorBoundsA = DoorA.GetBounds();
	const FBoxCenterAndExtent DoorBoundsB = DoorB.GetBounds();
	return Intersect(DoorBoundsA, DoorBoundsB);
}

bool IsPositionInside(const FDoorDef& Door, const FIntVector& Position)
{
	// If same position, then always inside.
	if (Door.Position == Position)
		return true;

	const FVector RoomUnit = Dungeon::RoomUnit();
	const FBoxCenterAndExtent DoorBounds = Door.GetBounds();
	const FVector LocalDoorExtent = DoorBounds.Extent / RoomUnit;
	const FVector RelativePosition = (FVector(Position) - (DoorBounds.Center / RoomUnit)).GetAbs();
	return RelativePosition.X <= LocalDoorExtent.X && RelativePosition.Y <= LocalDoorExtent.Y && RelativePosition.Z <= LocalDoorExtent.Z;
}

bool IsDoorValid(const URoomData* Data, const FDoorDef& Door)
{
	check(IsValid(Data));
	for (const FDoorDef& DoorDef : Data->Doors)
	{
		if (AreDoorsOverlapping(Door, DoorDef))
			return false;
	}
	return true;
}

void FProceduralDungeonEditorTool_Door::EnterTool()
{
	DungeonEd_LogInfo("Enter Door Tool.");
	UpdateRoomBox();
}

void FProceduralDungeonEditorTool_Door::ExitTool()
{
	DungeonEd_LogInfo("Exit Door Tool.");
	DestroyRoomBox();
	CachedLevel.Reset();
}

void FProceduralDungeonEditorTool_Door::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	FProceduralDungeonEditorTool::Render(View, Viewport, PDI);

	auto Level = EdMode->GetLevel();
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
		auto Level = EdMode->GetLevel();
		if (!Level.IsValid())
			return;

		URoomData* Data = Level->Data;
		check(IsValid(Data));

		UWorld* World = ViewportClient->GetWorld();
		FColor Color = IsDoorValid(Data, DoorPreview) ? FColor::Green : FColor::Orange;
		FDoorDef::DrawDebug(World, Color, DoorPreview, FTransform::Identity, /*includeOffset = */ true);
	}
}

bool FProceduralDungeonEditorTool_Door::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if (Click.IsAltDown() || Click.IsControlDown() || Click.IsShiftDown())
		return false;

	if (!ShowDoorPreview)
		return false;

	auto Level = EdMode->GetLevel();
	if (!Level.IsValid())
		return false;

	URoomData* Data = Level->Data;
	check(IsValid(Data));

	if (Click.GetKey() == EKeys::LeftMouseButton)
	{
		if (IsDoorValid(Data, DoorPreview))
		{
			GEditor->BeginTransaction(FText::FromString(TEXT("Add Door")));
			Data->Modify();
			Data->Doors.Add(DoorPreview);
			GEditor->EndTransaction();
			return true;
		}
	}

	if (Click.GetKey() == EKeys::RightMouseButton)
	{
		if (Data->Doors.Contains(DoorPreview))
		{
			GEditor->BeginTransaction(FText::FromString(TEXT("Remove Door")));
			Data->Modify();
			Data->Doors.Remove(DoorPreview);
			GEditor->EndTransaction();
		}
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

	auto Level = EdMode->GetLevel();
	if (!Level.IsValid())
		return false;

	URoomData* Data = Level->Data;
	check(IsValid(Data));

	// Snap preview to existing door if RoomCell is inside
	for (const FDoorDef& RoomDoor : Data->Doors)
	{
		if (RoomDoor.Direction != DoorPreview.Direction)
			continue;

		if (IsPositionInside(RoomDoor, DoorPreview.Position))
		{
			DoorPreview = RoomDoor;
			break;
		}
	}

	return false;
}

void FProceduralDungeonEditorTool_Door::OnLevelChanged(const ARoomLevel* NewLevel)
{
	UpdateRoomBox();
}

void FProceduralDungeonEditorTool_Door::OnDataChanged(const URoomData* NewData)
{
	UpdateRoomBox();
}

void FProceduralDungeonEditorTool_Door::UpdateRoomBox()
{
	auto Level = EdMode->GetLevelInstance();
	if (Level != CachedLevel)
	{
		CachedLevel = Level;

		DestroyRoomBox();
		if (CachedLevel.IsValid())
		{
			check(IsValid(Level->GetWorld()));
			RoomBox = NewObject<UBoxComponent>(CachedLevel.Get(), TEXT("Editor Room Collision"), RF_Transient);
			RoomBox->SetupAttachment(CachedLevel->GetRootComponent());
			RoomBox->RegisterComponent();
			RoomBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			RoomBox->SetCollisionObjectType(ECollisionChannel::ECC_MAX);
			DungeonEd_LogInfo("Create RoomBox: %s", *GetNameSafe(RoomBox.Get()));
		}
	}

	if (!CachedLevel.IsValid())
		return;

	FBoxCenterAndExtent Box = CachedLevel->Data->GetBounds();
	RoomBox->SetRelativeLocation(Box.Center);
	RoomBox->SetBoxExtent(Box.Extent);

	DungeonEd_LogInfo("Update RoomBox: %s", *GetNameSafe(RoomBox.Get()));
}

void FProceduralDungeonEditorTool_Door::DestroyRoomBox()
{
	if (RoomBox.IsValid())
	{
		RoomBox->DestroyComponent();
		RoomBox.Reset();
	}
}

bool FProceduralDungeonEditorTool_Door::RoomTraceFromMouse(FHitResult& OutHit, FEditorViewportClient* ViewportClient) const
{
	int32 MouseX = ViewportClient->Viewport->GetMouseX();
	int32 MouseY = ViewportClient->Viewport->GetMouseY();

	// Compute a world space ray from the screen space mouse coordinates
	FSceneViewFamilyContext ViewFamily(
		FSceneViewFamilyContext::ConstructionValues(ViewportClient->Viewport, ViewportClient->GetScene(), ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(ViewportClient->IsRealtime()));

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
	FVector RoomSpacePoint = Hit.ImpactPoint / Dungeon::RoomUnit();
	RoomSpacePoint -= 0.5f * (ToVector(OutDirection) + FVector::UpVector);
	OutCell = FIntVector(FMath::RoundToInt(RoomSpacePoint.X), FMath::RoundToInt(RoomSpacePoint.Y), FMath::RoundToInt(RoomSpacePoint.Z));
	return true;
}
