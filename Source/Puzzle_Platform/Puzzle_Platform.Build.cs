// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Puzzle_Platform : ModuleRules
{
	public Puzzle_Platform(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam" });
	}
}
