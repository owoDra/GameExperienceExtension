// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GEExt : ModuleRules
{
	public GEExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GEExt",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "GameFeatures",
                "ModularGameplay",
                "DeveloperSettings",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GFCore",
            }
        );
    }
}
