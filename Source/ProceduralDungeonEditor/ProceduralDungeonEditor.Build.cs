// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProceduralDungeonEditor : ModuleRules
{
	public ProceduralDungeonEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Uncomment that to detect when there are missing includes in cpp files
		//bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Projects" });
			
		PrivateDependencyModuleNames.AddRange(new string[] {
				"ProceduralDungeon",
				"CoreUObject",
				"InputCore",
				"PropertyEditor",
				"DetailCustomizations",
				"UnrealEd",
				"AssetTools",
				"ClassViewer",
				"SlateCore",
				"Slate",
				"EditorStyle",
#if UE_5_0_OR_LATER
				"EditorFramework",
#endif
				"Engine"
			}
		);
	}
}
