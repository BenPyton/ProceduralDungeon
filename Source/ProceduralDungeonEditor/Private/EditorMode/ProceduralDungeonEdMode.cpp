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
