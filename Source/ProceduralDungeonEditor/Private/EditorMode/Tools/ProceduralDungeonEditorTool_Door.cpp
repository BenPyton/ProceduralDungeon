// Copyright Benoit Pelletier 2023 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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

bool AreDoorsOverlapping(const FDoorDef& DoorA, const FDoorDef& DoorB, const FVector RoomUnit)
{
	// If not in same direction, they will never overlap.
	if (DoorA.Direction != DoorB.Direction)
		return false;

	// If same direction and position, then will always overlap.
	if (DoorA.Position == DoorB.Position)
		return true;

	const FBoxCenterAndExtent DoorBoundsA = DoorA.GetBounds(RoomUnit);
	const FBoxCenterAndExtent DoorBoundsB = DoorB.GetBounds(RoomUnit);
	return Intersect(DoorBoundsA, DoorBoundsB);
}

bool IsPositionInside(const FDoorDef& Door, const FIntVector& Position, const FVector RoomUnit)
{
	// If same position, then always inside.
	if (Door.Position == Position)
		return true;

	const FBoxCenterAndExtent DoorBounds = Door.GetBounds(RoomUnit);
	const FVector LocalDoorExtent = DoorBounds.Extent / RoomUnit;
	const FVector RelativePosition = (FVector(Position) - (DoorBounds.Center / RoomUnit)).GetAbs();
	return RelativePosition.X <= LocalDoorExtent.X && RelativePosition.Y <= LocalDoorExtent.Y && RelativePosition.Z <= LocalDoorExtent.Z;
}

bool IsDoorValid(const URoomData* Data, const FDoorDef& Door)
{
	check(IsValid(Data));
	for (const FDoorDef& DoorDef : Data->Doors)
	{
		if (AreDoorsOverlapping(Door, DoorDef, Data->GetRoomUnit()))
			return false;
	}
	return true;
}

void FProceduralDungeonEditorTool_Door::EnterTool()
{
	DungeonEd_LogInfo("Enter Door Tool.");
	UpdateCollision();
}

void FProceduralDungeonEditorTool_Door::ExitTool()
{
	DungeonEd_LogInfo("Exit Door Tool.");
	DestroyCollision();
	CachedLevel.Reset();
}

namespace
{
	void RenderBoundingBox(FPrimitiveDrawInterface* PDI, const FBoxCenterAndExtent& Box, const FColor& Color)
	{
		const FVector Center = Box.Center;
		const FVector Extent = Box.Extent;
		const FVector A = Center + FVector(Extent.X, Extent.Y, Extent.Z);
		const FVector B = Center + FVector(Extent.X, Extent.Y, -Extent.Z);
		const FVector C = Center + FVector(Extent.X, -Extent.Y, Extent.Z);
		const FVector D = Center + FVector(Extent.X, -Extent.Y, -Extent.Z);
		const FVector E = Center + FVector(-Extent.X, Extent.Y, Extent.Z);
		const FVector F = Center + FVector(-Extent.X, Extent.Y, -Extent.Z);
		const FVector G = Center + FVector(-Extent.X, -Extent.Y, Extent.Z);
		const FVector H = Center + FVector(-Extent.X, -Extent.Y, -Extent.Z);
		PDI->DrawLine(A, B, Color, SDPG_Foreground);
		PDI->DrawLine(A, C, Color, SDPG_Foreground);
		PDI->DrawLine(A, E, Color, SDPG_Foreground);
		PDI->DrawLine(B, D, Color, SDPG_Foreground);
		PDI->DrawLine(B, F, Color, SDPG_Foreground);
		PDI->DrawLine(C, D, Color, SDPG_Foreground);
		PDI->DrawLine(C, G, Color, SDPG_Foreground);
		PDI->DrawLine(D, H, Color, SDPG_Foreground);
		PDI->DrawLine(E, F, Color, SDPG_Foreground);
		PDI->DrawLine(E, G, Color, SDPG_Foreground);
		PDI->DrawLine(F, H, Color, SDPG_Foreground);
		PDI->DrawLine(G, H, Color, SDPG_Foreground);
	}

	void RenderInterlinesBoundingBox(FPrimitiveDrawInterface* PDI, const FBoxMinAndMax& Box, const FColor& Color, const FVector& RoomUnit)
	{
		FIntVector Min, Max;
		IntVector::MinMax(Box.GetMin(), Box.GetMax(), Min, Max);

		// Vertical Lines on X
		for (int32 i = Min.X + 1; i < Max.X; ++i)
		{
			FIntVector BottomA(i, Min.Y, Min.Z);
			FIntVector BottomB(i, Max.Y, Min.Z);
			FIntVector TopA(i, Min.Y, Max.Z);
			FIntVector TopB(i, Max.Y, Max.Z);
			PDI->DrawLine(Dungeon::ToWorldLocation(BottomA, RoomUnit), Dungeon::ToWorldLocation(TopA, RoomUnit), Color, SDPG_World);
			PDI->DrawLine(Dungeon::ToWorldLocation(BottomB, RoomUnit), Dungeon::ToWorldLocation(TopB, RoomUnit), Color, SDPG_World);
		}

		// Vertical Lines on Y
		for (int32 i = Min.Y + 1; i < Max.Y; ++i)
		{
			FIntVector BottomA(Min.X, i, Min.Z);
			FIntVector BottomB(Max.X, i, Min.Z);
			FIntVector TopA(Min.X, i, Max.Z);
			FIntVector TopB(Max.X, i, Max.Z);
			PDI->DrawLine(Dungeon::ToWorldLocation(BottomA, RoomUnit), Dungeon::ToWorldLocation(TopA, RoomUnit), Color, SDPG_World);
			PDI->DrawLine(Dungeon::ToWorldLocation(BottomB, RoomUnit), Dungeon::ToWorldLocation(TopB, RoomUnit), Color, SDPG_World);
		}

		// Horizontal Lines on X and Y
		for (int32 i = Min.Z + 1; i < Max.Z; ++i)
		{
			FIntVector A(Min.X, Min.Y, i);
			FIntVector B(Min.X, Max.Y, i);
			FIntVector C(Max.X, Max.Y, i);
			FIntVector D(Max.X, Min.Y, i);
			PDI->DrawLine(Dungeon::ToWorldLocation(A, RoomUnit), Dungeon::ToWorldLocation(B, RoomUnit), Color, SDPG_World);
			PDI->DrawLine(Dungeon::ToWorldLocation(B, RoomUnit), Dungeon::ToWorldLocation(C, RoomUnit), Color, SDPG_World);
			PDI->DrawLine(Dungeon::ToWorldLocation(C, RoomUnit), Dungeon::ToWorldLocation(D, RoomUnit), Color, SDPG_World);
			PDI->DrawLine(Dungeon::ToWorldLocation(D, RoomUnit), Dungeon::ToWorldLocation(A, RoomUnit), Color, SDPG_World);
		}
	}
}

void FProceduralDungeonEditorTool_Door::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	FProceduralDungeonEditorTool::Render(View, Viewport, PDI);

	const URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return;

	const FColor LineColor(100, 20, 0);
	const FVector RoomUnit = Data->GetRoomUnit();

	for (const auto& BoundingBox : Data->BoundingBoxes)
	{
		RenderInterlinesBoundingBox(PDI, BoundingBox, LineColor, RoomUnit);
	}
}

void FProceduralDungeonEditorTool_Door::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FProceduralDungeonEditorTool::Tick(ViewportClient, DeltaTime);

	if (ShowDoorPreview)
	{
		const URoomData* Data = GetRoomData();
		check(IsValid(Data));

		UWorld* World = ViewportClient->GetWorld();
		FDoorDef::DrawDebug(World, DoorPreview, Data->GetRoomUnit(), FTransform::Identity, /*includeOffset = */ true, /*isConnected = */ IsDoorValid(Data, DoorPreview));
	}
}

bool FProceduralDungeonEditorTool_Door::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if (Click.IsAltDown() || Click.IsControlDown() || Click.IsShiftDown())
		return false;

	if (!ShowDoorPreview)
		return false;

	URoomData* Data = GetRoomData();
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
			return true;
		}
	}

	return false;
}

bool FProceduralDungeonEditorTool_Door::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY)
{
	ShowDoorPreview = false;

	const URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return false;

	FHitResult Hit;
	if (!RoomTraceFromMouse(Hit, ViewportClient))
		return false;

	FIntVector RoomCell;
	EDoorDirection DoorDirection;
	if (!GetRoomCellFromHit(Hit, Data->GetRoomUnit(), RoomCell, DoorDirection))
		return false;

	ShowDoorPreview = true;
	DoorPreview.Position = RoomCell;
	DoorPreview.Direction = DoorDirection;
	DoorPreview.Type = EdMode->Settings->DoorType;

	// Snap preview to existing door if RoomCell is inside
	for (const FDoorDef& RoomDoor : Data->Doors)
	{
		if (RoomDoor.Direction != DoorPreview.Direction)
			continue;

		if (IsPositionInside(RoomDoor, DoorPreview.Position, Data->GetRoomUnit()))
		{
			DoorPreview = RoomDoor;
			break;
		}
	}

	return false;
}

bool FProceduralDungeonEditorTool_Door::GetCursor(EMouseCursor::Type& OutCursor) const
{
	if (!ShowDoorPreview)
		return false;

	const URoomData* Data = GetRoomData();
	if (!IsValid(Data))
		return false;

	OutCursor = IsDoorValid(Data, DoorPreview) ? EMouseCursor::Hand : EMouseCursor::SlashedCircle;
	return true;
}

void FProceduralDungeonEditorTool_Door::OnLevelChanged(const ARoomLevel* NewLevel)
{
	UpdateCollision();
}

void FProceduralDungeonEditorTool_Door::OnDataChanged(const URoomData* NewData)
{
	UpdateCollision();
}

void FProceduralDungeonEditorTool_Door::OnDataPropertiesChanged(const URoomData* Data)
{
	UpdateCollision();
}

void FProceduralDungeonEditorTool_Door::CreateCollision(ARoomLevel* Level)
{
	if (!IsValid(Level))
		return;

	if (!IsValid(Level->Data))
		return;

	check(IsValid(Level->GetWorld()));

	const int32 NumBoxes = Level->Data->BoundingBoxes.Num();
	const int32 Delta = NumBoxes - RoomBoxes.Num();

	// Create new boxes when delta is positive
	for (int i = 0; i < Delta; ++i)
	{
		FName BoxName = FName(*FString::Printf(TEXT("Editor Room Collision %d"), RoomBoxes.Num()));
		UBoxComponent* RoomBox = NewObject<UBoxComponent>(Level, BoxName, RF_Transient);
		check(IsValid(RoomBox));
		RoomBox->SetupAttachment(Level->GetRootComponent());
		RoomBox->RegisterComponent();
		RoomBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		RoomBox->SetCollisionObjectType(ECollisionChannel::ECC_MAX);
		RoomBoxes.Add(RoomBox);
		DungeonEd_LogInfo("Created RoomBox: %s", *GetNameSafe(RoomBox));
	}

	// Destroy boxes when delta is negative
	for (int i = 0; i > Delta; --i)
	{
		if (RoomBoxes.Num() == 0)
			break;
		TWeakObjectPtr<UBoxComponent> RoomBox = RoomBoxes.Pop();
		if (!RoomBox.IsValid())
			continue;
		RoomBox->DestroyComponent();
		DungeonEd_LogInfo("Destroyed RoomBox: %s", *GetNameSafe(RoomBox.Get()));
	}
}

void FProceduralDungeonEditorTool_Door::UpdateCollision()
{
	auto Level = EdMode->GetLevelInstance();
	if (Level != CachedLevel)
	{
		DestroyCollision();
		CachedLevel = Level;
	}


	if (!CachedLevel.IsValid())
		return;

	URoomData* Data = CachedLevel->Data;
	if (!IsValid(Data))
		return;

	CreateCollision(CachedLevel.Get());
	check(Data->BoundingBoxes.Num() == RoomBoxes.Num());

	for (int i = 0; i < Data->BoundingBoxes.Num(); ++i)
	{
		const FBoxMinAndMax& DataBox = Data->BoundingBoxes[i];
		UBoxComponent* RoomBox = RoomBoxes[i].Get();

		FBoxCenterAndExtent Box = Dungeon::ToWorld(DataBox, Data->GetRoomUnit());
		RoomBox->SetRelativeLocation(Box.Center);
		RoomBox->SetBoxExtent(Box.Extent);

		DungeonEd_LogInfo("Updated RoomBox: %s", *GetNameSafe(RoomBox));
	}
}

void FProceduralDungeonEditorTool_Door::DestroyCollision()
{
	for (const auto& RoomBox : RoomBoxes)
	{
		if (!RoomBox.IsValid())
			continue;

		RoomBox->DestroyComponent();
	}

	RoomBoxes.Empty();
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
	OutHit = FHitResult();
	FHitResult Hit;
	bool bHasHit = false;
	for (const auto& RoomBox : RoomBoxes)
	{
		if (!RoomBox.IsValid())
			continue;

		const bool bSuccess = RoomBox->LineTraceComponent(Hit, RayOrigin, RayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(RoomTrace)));
		if (!bSuccess)
			continue;

		if (!bHasHit || Hit.Distance < OutHit.Distance)
		{
			OutHit = Hit;
			bHasHit = true;
		}
	}

	return bHasHit;
}

bool FProceduralDungeonEditorTool_Door::GetRoomCellFromHit(const FHitResult& Hit, const FVector RoomUnit, FIntVector& OutCell, EDoorDirection& OutDirection) const
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
	FVector RoomSpacePoint = Hit.ImpactPoint / RoomUnit;
	RoomSpacePoint -= 0.5f * (ToVector(OutDirection) + FVector::UpVector);
	OutCell = FIntVector(FMath::RoundToInt(RoomSpacePoint.X), FMath::RoundToInt(RoomSpacePoint.Y), FMath::RoundToInt(RoomSpacePoint.Z));
	return true;
}
