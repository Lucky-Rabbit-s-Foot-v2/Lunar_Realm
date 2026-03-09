// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/Option/OptionManagerSubsystem.h"

#include "Engine/GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

#include "Subsystems/Option/GraphicSubsystem.h"
#include "Subsystems/Option/SoundSubsystem.h"

void UOptionManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UGraphicSubsystem>();
	Collection.InitializeDependency<USoundSubsystem>();

	Super::Initialize(Collection);
	LR_INFO(TEXT("OptionManager Initialized"));

	LoadOptions();

	InitializeOptions();
}

void UOptionManagerSubsystem::Deinitialize()
{
	SaveOptions();
	
	Super::Deinitialize();
}

void UOptionManagerSubsystem::UpdateCurrentOptionSaveGame()
{
	if (UGraphicSubsystem* GraphicSubsys = GetGameInstance()->GetSubsystem<UGraphicSubsystem>())
	{
		CurrentOptionSaveGame->SetGraphicsOptions(GraphicSubsys->GetCurrentOptions());
		GraphicSubsys->ApplyOptions();
	}

	if (USoundSubsystem* SoundSubsys = GetGameInstance()->GetSubsystem<USoundSubsystem>())
	{
		CurrentOptionSaveGame->SetSoundOptions(SoundSubsys->GetCurrentOptions());
		SoundSubsys->ApplyOptions();
	}

	SaveOptions();
}

void UOptionManagerSubsystem::UpdateOptionValue(ESettingType SettingType, int32 Value)
{
	UGraphicSubsystem* GraphicSubsys = GetGameInstance()->GetSubsystem<UGraphicSubsystem>();
	USoundSubsystem* SoundSubsys = GetGameInstance()->GetSubsystem<USoundSubsystem>();

	LR_INFO(TEXT("Updating Option Value - Type: %d, Value: %d"), static_cast<uint8>(SettingType), Value);

	switch (SettingType)
	{
	case ESettingType::TEXTURE:				GraphicSubsys->SetTextureQuality(static_cast<EGraphicOptionLevel>(Value)); break;
	case ESettingType::SHADOW:				GraphicSubsys->SetShadowQuality(static_cast<EGraphicOptionLevel>(Value)); break;
	case ESettingType::ANTI_ALIASING:		GraphicSubsys->SetAntiAliasingQuality(static_cast<EGraphicOptionLevel>(Value)); break;
	case ESettingType::POST_PROCESSING:		GraphicSubsys->SetPostProcessingQuality(static_cast<EGraphicOptionLevel>(Value)); break;
	case ESettingType::VFX:					GraphicSubsys->SetVisualEffectQuality(static_cast<EGraphicOptionLevel>(Value)); break;
	case ESettingType::RESOLUTION_SCALE:	GraphicSubsys->SetResolutionScale(Value); break;
	case ESettingType::FRAME_RATE:			GraphicSubsys->SetFrameRateLimit(Value); break;

	case ESettingType::MASTER_SOUND:		SoundSubsys->SetVolume(ESoundChannel::Master, Value); break;
	case ESettingType::BGM:					SoundSubsys->SetVolume(ESoundChannel::BGM, Value); break;
	case ESettingType::SFX:					SoundSubsys->SetVolume(ESoundChannel::SFX, Value); break;

	default: break;
	}
}

void UOptionManagerSubsystem::CreateNewOptionSaveData()
{
	CurrentOptionSaveGame = Cast<ULROptionSaveGame>(UGameplayStatics::CreateSaveGameObject(ULROptionSaveGame::StaticClass()));
	CurrentOptionSaveGame->ApplyDefaults();
}

void UOptionManagerSubsystem::InitializeOptions()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGraphicSubsystem* Subsys = GI->GetSubsystem<UGraphicSubsystem>())
		{
			Subsys->InitializeFromSaveData(CurrentOptionSaveGame->GetGraphicOptions());
		}

		if (USoundSubsystem* Subsys = GI->GetSubsystem<USoundSubsystem>())
		{
			Subsys->InitializeFromSaveData(CurrentOptionSaveGame->GetSoundOptions());
		}
	}
}

void UOptionManagerSubsystem::LoadOptions()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		CurrentOptionSaveGame = Cast<ULROptionSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
		if (!ensureMsgf(CurrentOptionSaveGame, TEXT("Could not load Game Option Save Slot")))
		{
			return;
		}

		FGraphicOptionData LoadedGraphicOptions = CurrentOptionSaveGame->GetGraphicOptions();
		FSoundOptionData LoadedSoundOptions = CurrentOptionSaveGame->GetSoundOptions();

		LR_INFO(TEXT("Loaded Game Option Successfully - Texture: %d, Shadow: %d, AA: %d, PostProcess: %d, VFX: %d, ResScale: %f, FrameRate: %f, MasterSound: %f, BGM: %f, SFX: %f"),
			LoadedGraphicOptions.TextureQuality,
			LoadedGraphicOptions.ShadowQuality,
			LoadedGraphicOptions.AntiAliasingQuality,
			LoadedGraphicOptions.PostProcessingQuality,
			LoadedGraphicOptions.VisualEffectQuality,
			LoadedGraphicOptions.ResolutionScale,
			LoadedGraphicOptions.FrameRateLimit,
			LoadedSoundOptions.MasterVolume,
			LoadedSoundOptions.BGMVolume,
			LoadedSoundOptions.SFXVolume
		);

	}
	else
	{
		LR_INFO(TEXT("No Save Game Option found, creating new one"));
		CreateNewOptionSaveData();
		SaveOptions();
	}
}

void UOptionManagerSubsystem::SaveOptions()
{
	if (!ensureMsgf(CurrentOptionSaveGame, TEXT("CANNOT SAVE : CurrentOptionSaveGame is NULL")))
	{
		return;
	}

	if (UGameplayStatics::SaveGameToSlot(CurrentOptionSaveGame, SaveSlotName, UserIndex))
	{
		LR_INFO(TEXT("Save Game Option Successfully"));
	}
}
