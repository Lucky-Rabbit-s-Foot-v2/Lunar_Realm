// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/GraphicSubsystem.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UGraphicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->LoadSettings();
		UserSettings->ApplySettings(false);
	}
}

void UGraphicSubsystem::Deinitialize()
{
	ApplyAndSave();
	Super::Deinitialize();
}

void UGraphicSubsystem::SetTextureQuality(EGraphicOptionLevel Level)
{
	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetTextureQuality(QualityLevel);
	}
}

void UGraphicSubsystem::SetShadowQuality(EGraphicOptionLevel Level)
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetShadowQuality(QualityLevel);
	}
}

void UGraphicSubsystem::SetAntiAliasingQuality(EGraphicOptionLevel Level)
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetAntiAliasingQuality(QualityLevel);
	}
}

void UGraphicSubsystem::SetPostProcessingQuality(EGraphicOptionLevel Level)
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetPostProcessingQuality(QualityLevel);
	}
}

void UGraphicSubsystem::SetVisualEffectQuality(EGraphicOptionLevel Level)
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetVisualEffectQuality(QualityLevel);
	}
}

void UGraphicSubsystem::SetResolutionScale(float Percent)
{
	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		float ClampedValue = FMath::Clamp(Percent, 50.0f, 100.0f);
		UserSettings->SetResolutionScaleValueEx(ClampedValue);
	}
}

void UGraphicSubsystem::SetFrameRateLimit(float Limit)
{
	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->SetFrameRateLimit(Limit);
	}
}

void UGraphicSubsystem::SetOverallQuality(EGraphicOptionLevel Level)
{
	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		int32 QualityLevel = ConvertLevelToInt(Level);
		UserSettings->SetOverallScalabilityLevel(QualityLevel);
	}
}

void UGraphicSubsystem::ApplyAndSave()
{
	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->ApplySettings(false);
		UserSettings->SaveSettings();
	}
}

void UGraphicSubsystem::RunAutoBenchmark()
{
	if(UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->RunHardwareBenchmark();
		UserSettings->ApplyHardwareBenchmarkResults();
		ApplyAndSave();
	}
}

int32 UGraphicSubsystem::ConvertLevelToInt(EGraphicOptionLevel Level) const
{
	int32 Value = static_cast<int32>(Level);

#if PLATFORM_ANDROID
	if (Value > 2)
	{
		Value = 2;
	}
#endif
	return Value;
}
