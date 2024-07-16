// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

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
