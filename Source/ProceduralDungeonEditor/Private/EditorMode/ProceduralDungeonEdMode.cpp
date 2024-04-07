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
#include "Engine/LevelScriptBlueprint.h"
#include "ProceduralDungeonEdModeToolkit.h"
#include "ProceduralDungeonEdLog.h"
#include "ProceduralDungeonEditorSettings.h"
#include "ProceduralDungeonEditorObject.h"
#include "Tools/ProceduralDungeonEditorTool_Size.h"
#include "Tools/ProceduralDungeonEditorTool_Door.h"
#include "Room.h"
#include "RoomLevel.h"
#include "RoomData.h"

#define ROUTE_TO_TOOL(FuncCall) ActiveTool ? ActiveTool->FuncCall : FEdMode::FuncCall

const FEditorModeID FProceduralDungeonEdMode::EM_ProceduralDungeon(TEXT("EM_ProceduralDungeon"));

FProceduralDungeonEdMode::FProceduralDungeonEdMode()
	: FEdMode()
{
	Tools.Add(MakeUnique<FProceduralDungeonEditorTool_Size>(this));
	Tools.Add(MakeUnique<FProceduralDungeonEditorTool_Door>(this));

	Settings = NewObject<UProceduralDungeonEditorObject>(GetTransientPackage(), TEXT("Editor Settings"), RF_Transactional);
}

void FProceduralDungeonEdMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdMode::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(Settings);
}

void FProceduralDungeonEdMode::Enter()
{
	DungeonEd_LogInfo("Enter Room Editor Mode.");
	FEdMode::Enter();

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FProceduralDungeonEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}

	UpdateLevelBlueprint();
}

void FProceduralDungeonEdMode::Exit()
{
	RegisterLevelCompilationDelegate(false);

	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();

	if (ActiveTool)
	{
		ActiveTool->ExitTool();
		ActiveTool = nullptr;
	}

	CachedLevelInstance.Reset();
	CachedLevelBlueprint.Reset();

	FEdMode::Exit();
	DungeonEd_LogInfo("Exit Room Editor Mode.");
}

void FProceduralDungeonEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	if (ActiveTool)
		ActiveTool->Render(View, Viewport, PDI);

	FEdMode::Render(View, Viewport, PDI);
}

void FProceduralDungeonEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);

	if (ActiveTool)
		ActiveTool->Tick(ViewportClient, DeltaTime);
}

bool FProceduralDungeonEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	return ROUTE_TO_TOOL(HandleClick(InViewportClient, HitProxy, Click));
}

bool FProceduralDungeonEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	return ROUTE_TO_TOOL(InputKey(ViewportClient, Viewport, Key, Event));
}

bool FProceduralDungeonEdMode::InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime)
{
	return ROUTE_TO_TOOL(InputAxis(InViewportClient, Viewport, ControllerId, Key, Delta, DeltaTime));
}

bool FProceduralDungeonEdMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	return ROUTE_TO_TOOL(InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale));
}

bool FProceduralDungeonEdMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY)
{
	return ROUTE_TO_TOOL(MouseMove(ViewportClient, Viewport, MouseX, MouseY));
}

bool FProceduralDungeonEdMode::ShowModeWidgets() const
{
	return true;
}

bool FProceduralDungeonEdMode::ShouldDrawWidget() const
{
	return true;
}

bool FProceduralDungeonEdMode::UsesTransformWidget() const
{
	return ROUTE_TO_TOOL(UsesTransformWidget());
}

bool FProceduralDungeonEdMode::UsesTransformWidget(WidgetMode CheckMode) const
{
	return ROUTE_TO_TOOL(UsesTransformWidget(CheckMode));
}

FVector FProceduralDungeonEdMode::GetWidgetLocation() const
{
	return ROUTE_TO_TOOL(GetWidgetLocation());
}

bool FProceduralDungeonEdMode::GetPivotForOrbit(FVector& OutPivot) const
{
	const auto* PluginSettings = GetDefault<UProceduralDungeonEditorSettings>();
	if (!PluginSettings->bUseRoomAsOrbitPivot)
		return false;

	if (!CachedLevelInstance.IsValid())
		return false;

	URoomData* Data = CachedLevelInstance->Data;
	if (!IsValid(Data))
		return false;

	FBoxCenterAndExtent RoomBounds = Data->GetBounds();
	OutPivot = RoomBounds.Center;
	return true;
}

bool FProceduralDungeonEdMode::GetTool(FName ToolName, FProceduralDungeonEditorTool*& OutTool) const
{
	for (auto& Tool : Tools)
	{
		if (Tool.IsValid() && Tool->GetToolName() == ToolName)
		{
			OutTool = Tool.Get();
			return true;
		}
	}
	return false;
}

FProceduralDungeonEditorTool* FProceduralDungeonEdMode::GetActiveTool() const
{
	return ActiveTool;
}

void FProceduralDungeonEdMode::SetActiveTool(FName ToolName)
{
	if (ActiveTool && ActiveTool->GetToolName() == ToolName)
		return;

	FProceduralDungeonEditorTool* NewTool = nullptr;
	if (!GetTool(ToolName, NewTool))
	{
		DungeonEd_LogError("Tool '%s' is not a valid tool.", *ToolName.ToString());
		return;
	}

	check(NewTool);
	SetActiveTool(NewTool);
}

void FProceduralDungeonEdMode::ResetActiveTool()
{
	SetActiveTool(nullptr);
}

void FProceduralDungeonEdMode::SetActiveTool(FProceduralDungeonEditorTool* NewTool)
{
	if (ActiveTool)
		ActiveTool->ExitTool();

	DungeonEd_LogInfo("Set active tool to '%s'.", NewTool ? NewTool->GetToolName() : TEXT("None"));
	ActiveTool = NewTool;

	if (ActiveTool)
		ActiveTool->EnterTool();
}

void FProceduralDungeonEdMode::SetDefaultTool()
{
	if (!ActiveTool && IsToolEnabled("Tool_Size"))
		SetActiveTool("Tool_Size");
}

bool FProceduralDungeonEdMode::IsToolEnabled(FName ToolName) const
{
	auto Level = GetLevel();
	return Level.IsValid() && IsValid(Level->Data);
}

ULevelScriptBlueprint* FProceduralDungeonEdMode::GetLevelBlueprint(bool bCreate) const
{
	UWorld* World = GetWorld();
	check(World);
	ULevelScriptBlueprint* LevelBlueprint = World->PersistentLevel->GetLevelScriptBlueprint(/*bDontCreate = */!bCreate);
	return LevelBlueprint;
}

TWeakObjectPtr<ARoomLevel> FProceduralDungeonEdMode::GetLevel() const
{
	ULevelScriptBlueprint* LevelBlueprint = GetLevelBlueprint();
	if (!IsValid(LevelBlueprint))
		return nullptr;
	return Cast<ARoomLevel>(LevelBlueprint->GeneratedClass->GetDefaultObject());
}

void FProceduralDungeonEdMode::UpdateLevelBlueprint()
{
	ULevelScriptBlueprint* LevelBlueprint = GetLevelBlueprint();
	if (CachedLevelBlueprint == LevelBlueprint)
		return;

	if (CachedLevelBlueprint.IsValid())
		RegisterLevelCompilationDelegate(false);

	CachedLevelBlueprint = LevelBlueprint;

	if (CachedLevelBlueprint.IsValid())
		RegisterLevelCompilationDelegate(true);

	OnLevelBlueprintCompiled();
}

void FProceduralDungeonEdMode::OnLevelBlueprintCompiled(UBlueprint* Blueprint)
{
	CachedLevelInstance = Cast<ARoomLevel>(GetWorld()->GetLevelScriptActor());

	auto Level = GetLevel();
	DungeonEd_LogInfo("Room Level: %s", *GetNameSafe(Level.Get()));

	if (Level.IsValid())
		SetDefaultTool();
	else
		ResetActiveTool();

	auto RoomToolkit = (FProceduralDungeonEdModeToolkit*)Toolkit.Get();
	check(RoomToolkit);
	RoomToolkit->OnLevelChanged();

	if (ActiveTool)
		ActiveTool->OnLevelChanged(Level.Get());
}

void FProceduralDungeonEdMode::RegisterLevelCompilationDelegate(bool Register)
{
	if (!CachedLevelBlueprint.IsValid())
	{
		DungeonEd_LogWarning("Can't (un)register level blueprint compilation delegate: the level blueprint is invalid.");
		return;
	}

	if (Register)
	{
		if (LevelBlueprintDelegateHandle.IsValid())
		{
			DungeonEd_LogWarning("Can't register level blueprint compilation delegate: the delegate is already registered.");
		}
		else
		{
			LevelBlueprintDelegateHandle = CachedLevelBlueprint->OnCompiled().AddRaw(this, &FProceduralDungeonEdMode::OnLevelBlueprintCompiled);
			DungeonEd_LogInfo("Regitered level blueprint compilation delegate.");
		}
	}
	else
	{
		if (LevelBlueprintDelegateHandle.IsValid())
		{
			CachedLevelBlueprint->OnCompiled().Remove(LevelBlueprintDelegateHandle);
			LevelBlueprintDelegateHandle.Reset();
			DungeonEd_LogInfo("Unregitered level blueprint compilation delegate.");
		}
		else
		{
			DungeonEd_LogWarning("Can't unregister level blueprint compilation delegate: the delegate is not registered.");
		}
	}
}
