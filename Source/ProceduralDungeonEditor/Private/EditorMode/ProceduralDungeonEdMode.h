#pragma once

#include "EdMode.h"

class FProceduralDungeonEdMode : public FEdMode
{
public:
    const static FEditorModeID EM_ProceduralDungeon;

    // FEdMode interface
    virtual void Enter() override;
    virtual void Exit() override;
};