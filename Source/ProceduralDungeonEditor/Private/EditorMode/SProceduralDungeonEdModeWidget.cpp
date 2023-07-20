#include "SProceduralDungeonEdModeWidget.h"
#include "ProceduralDungeonEditor.h"
#include "ProceduralDungeonEdMode.h"
#include "Widgets/Layout/SScrollBox.h"

void SProceduralDungeonEdModeWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .VAlign(VAlign_Top)
            .Padding(5.f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("This is a editor mode example.")))
            ]
        ];
}

FProceduralDungeonEdMode* SProceduralDungeonEdModeWidget::GetEdMode() const
{
    return (FProceduralDungeonEdMode*)GLevelEditorModeTools().GetActiveMode(FProceduralDungeonEdMode::EM_ProceduralDungeon);
}