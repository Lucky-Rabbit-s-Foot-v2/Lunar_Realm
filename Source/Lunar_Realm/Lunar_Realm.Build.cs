// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lunar_Realm : ModuleRules
{
	public Lunar_Realm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivatePCHHeaderFile = "Public/Lunar_Realm.h";
		
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine",
            "InputCore", "EnhancedInput",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks", "GameplayTags", "GameplayAbilities",
            "Slate", "SlateCore", "UMG",
            "Niagara", "NiagaraCore",
            "VectorVM",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
