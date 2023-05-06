// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProceduralDungeonEditor : ModuleRules
{
	public ProceduralDungeonEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Projects" });
			
		PrivateDependencyModuleNames.AddRange(new string[] {
				"ProceduralDungeon",
				"CoreUObject",
				"PropertyEditor",
				"DetailCustomizations",
				"UnrealEd",
				"AssetTools",
				"ClassViewer"
			}
		);
	}
}
