/*
 * Copyright (c) 2019-2024 Benoit Pelletier
 *
 * This software is available under different licenses depending on the source from which it was obtained:
 * - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
 * - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
 *
 * Please refer to the accompanying LICENSE file for further details.
 */

using UnrealBuildTool;

public class ProceduralDungeon : ModuleRules
{
	public ProceduralDungeon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Uncomment that to detect when there are missing includes in cpp files
		//bUseUnity = false;
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"NavigationSystem",
#if UE_5_2_OR_LATER
			"IrisCore",
#endif
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Engine",
			"CoreUObject",
			"NetCore",
		});
	}
}
