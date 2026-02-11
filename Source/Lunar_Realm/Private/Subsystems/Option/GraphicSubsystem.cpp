// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/Option/GraphicSubsystem.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Data/LROptionDataStructs.h"

void UGraphicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->LoadSettings();
	}
}

void UGraphicSubsystem::InitializeFromSaveData(const FGraphicOptionData& NewLoadedOptions)
{
	CurrentOptions = NewLoadedOptions;
	ApplyOptions();
}

void UGraphicSubsystem::ApplyOptions()
{

	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings) return;
	
	// 1. 퀄리티 설정 적용
	UserSettings->SetTextureQuality(CurrentOptions.TextureQuality);
	UserSettings->SetShadowQuality(CurrentOptions.ShadowQuality);
	UserSettings->SetAntiAliasingQuality(CurrentOptions.AntiAliasingQuality);
	UserSettings->SetPostProcessingQuality(CurrentOptions.PostProcessingQuality);
	UserSettings->SetVisualEffectQuality(CurrentOptions.VisualEffectQuality);

	// 2. 렌더링 해상도 (50% ~ 100%)
	const float NewResScale = FMath::Clamp(CurrentOptions.ResolutionScale, 50.f, 100.f);
	UserSettings->SetResolutionScaleValueEx(NewResScale);

	// 3. 프레임 제한 (30, 60, 120)
	const float NewFrameRate = FMath::Clamp(CurrentOptions.FrameRateLimit, 30.f, 120.f);
	UserSettings->SetFrameRateLimit(NewFrameRate);

	UserSettings->SaveSettings();
}

int32 UGraphicSubsystem::ConvertLevelToInt(EGraphicOptionLevel Level) const
{
	return FMath::Clamp(static_cast<int32>(Level), 0, 2);
}
