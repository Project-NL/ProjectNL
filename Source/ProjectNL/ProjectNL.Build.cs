// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectNL : ModuleRules
{
	public ProjectNL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"UMG", "GameplayAbilities", "GameplayTags", "GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new[] { "Slate", "SlateCore","AIModule","NavigationSystem", "Niagara", "Niagara" });
	}
}
