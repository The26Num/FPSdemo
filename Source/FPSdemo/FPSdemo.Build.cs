// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSdemo : ModuleRules
{
	public FPSdemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG"});
    }
}
