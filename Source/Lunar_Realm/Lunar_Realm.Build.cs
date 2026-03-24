// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
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
            "MediaAssets",
            "AssetRegistry", "RenderCore"
            // UnrealEd, Blutility, UMGEditor 제거
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // 안드로이드 빌드를 위해 에디터 전용 모듈은 에디터 빌드일 때만 포함되도록 조건부로 추가
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd",
                "Blutility",
                "UMGEditor"
            });
        }

        // Android UPL 등록
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            AdditionalPropertiesForReceipt.Add(
                "AndroidPlugin",
                Path.Combine(ModuleDirectory, "Lunar_Realm_UPL_Android.xml")
            );
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
