﻿// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GEExtEditor : ModuleRules
{
	public GEExtEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
            {
                ModuleDirectory,
                ModuleDirectory + "/GEExtEditor",
            }
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "UnrealEd",
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "AssetTools", 

                "GEExt",
            }
		);
    }
}
