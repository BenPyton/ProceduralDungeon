#pragma once

#include "Framework/Application/SlateApplication.h"

class SProceduralDungeonEdModeWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProceduralDungeonEdModeWidget) {}
    SLATE_END_ARGS();

    void Construct(const FArguments& InArgs);

    // Util Functions
    class FProceduralDungeonEdMode* GetEdMode() const;
};