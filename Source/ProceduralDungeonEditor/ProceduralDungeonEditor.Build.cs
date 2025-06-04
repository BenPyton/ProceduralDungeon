// Copyright Benoit Pelletier 2019 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

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
