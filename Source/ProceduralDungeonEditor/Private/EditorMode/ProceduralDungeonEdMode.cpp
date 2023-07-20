#include "ProceduralDungeonEdMode.h"
#include "ProceduralDungeonEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "ScopedTransaction.h"
#include "ProceduralDungeonEdModeToolkit.h"

const FEditorModeID FProceduralDungeonEdMode::EM_ProceduralDungeon(TEXT("EM_ProceduralDungeon"));

void FProceduralDungeonEdMode::Enter()
{
    FEdMode::Enter();

    if (!Toolkit.IsValid())
    {
        Toolkit = MakeShareable(new FExampleEdModeToolkit);
        Toolkit->Init(Owner->GetToolkitHost());
    }
}

void FProceduralDungeonEdMode::Exit()
{
    FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
    Toolkit.Reset();

    FEdMode::Exit();
}