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

#if ENGINE_MAJOR_VERSION <= 4
#define COMPATIBILITY 1
#else
#define COMPATIBILITY 0
#endif

#include "EdMode.h"
#if !COMPATIBILITY
#include "UnrealWidgetFwd.h"
#endif

#if COMPATIBILITY
using WidgetMode = FWidget::EWidgetMode;
#else
using WidgetMode = UE::Widget::EWidgetMode;
#endif

class ARoomLevel;

struct HRoomPointProxy : public HHitProxy
{
    DECLARE_HIT_PROXY();

    HRoomPointProxy(int32 InIndex)
        : HHitProxy(HPP_UI), Index(InIndex)
    {}

    int32 Index {-1};
};

EAxisList::Type operator~(const EAxisList::Type& This)
{
    return static_cast<EAxisList::Type>(EAxisList::All - This);
}

EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other)
{
    This = static_cast<EAxisList::Type>(This & Other);
    return This;
}

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

class FProceduralDungeonEdMode : public FEdMode
{
public:
    const static FEditorModeID EM_ProceduralDungeon;

    // FEdMode interface
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
    virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
    virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
    virtual bool ShowModeWidgets() const override;
    virtual bool ShouldDrawWidget() const override;
    virtual bool UsesTransformWidget() const override;
    virtual bool UsesTransformWidget(WidgetMode CheckMode) const;
    virtual FVector GetWidgetLocation() const override;
    virtual bool AllowWidgetMove() override { return true; }

    bool HasValidSelection() const;
    void UpdateDataAsset() const;

    TWeakObjectPtr<ARoomLevel> Level = nullptr;
    TWeakObjectPtr<UWorld> World = nullptr;
    int32 SelectedPoint {-1};
    TArray<FRoomPoint> Points;
    FVector DragPoint;
};