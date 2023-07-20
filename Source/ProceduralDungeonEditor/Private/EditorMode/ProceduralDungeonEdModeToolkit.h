#pragma once

#include "Toolkits/BaseToolkit.h"
#include "ProceduralDungeonEdMode.h"
#include "SProceduralDungeonEdModeWidget.h"

class FExampleEdModeToolkit : public FModeToolkit
{
public:
    FExampleEdModeToolkit()
    {
        SAssignNew(EdModeWidget, SProceduralDungeonEdModeWidget);
    }

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override { return FName("ProceduralDungeonEdMode"); }
    virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("ProceduralDungeonEdModeToolkit", "DisplayName", "ProceduralDungeonEdMode Tool"); }
    virtual class FEdMode* GetEditorMode() const override { return GLevelEditorModeTools().GetActiveMode(FProceduralDungeonEdMode::EM_ProceduralDungeon); }
    virtual TSharedPtr<class SWidget> GetInlineContent() const override { return EdModeWidget; }

private:
    TSharedPtr<SProceduralDungeonEdModeWidget> EdModeWidget;
};