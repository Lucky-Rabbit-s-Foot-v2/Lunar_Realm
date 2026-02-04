// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SoundSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/LROptionSaveGame.h"

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GlobalSoundMix)
	{
		UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	}
	
	LoadVolumesFromSaveData();
}

void USoundSubsystem::PlayBGM(USoundBase* NewBGM, float FadeTime)
{
	if (!NewBGM)
	{
		return;
	}

	if (CurrentBGMComp && CurrentBGMComp->Sound == NewBGM && CurrentBGMComp->IsPlaying())
	{
		return;
	}

	StopBGM(FadeTime);

	CurrentBGMComp = UGameplayStatics::CreateSound2D(GetWorld(), NewBGM);
	if (CurrentBGMComp)
	{
		CurrentBGMComp->bAutoDestroy = false;
		CurrentBGMComp->FadeIn(FadeTime);
	}
}

void USoundSubsystem::StopBGM(float FadeTime)
{
	if (CurrentBGMComp && CurrentBGMComp->IsPlaying())
	{
		CurrentBGMComp->FadeOut(FadeTime, 0.f);
	}
}

void USoundSubsystem::PlaySFX_2D(USoundBase* Sound, float Volume, float Pitch)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, Volume, Pitch);
	}
}

void USoundSubsystem::PlaySFX_AtLocation(USoundBase* Sound, FVector Location, float Volume, float Pitch)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume, Pitch);
	}

}

void USoundSubsystem::SetVolume(ESoundChannel Channel, float Volume)
{
	if (!GlobalSoundMix)
	{
		return;
	}

	USoundClass* TargetClass = nullptr;
	switch (Channel)
	{
	case ESoundChannel::Master:
		TargetClass = MasterClass;
		CurrentMasterVolume = Volume;
		break;
	case ESoundChannel::BGM:
		TargetClass = BGMClass;
		CurrentBGMVolume = Volume;
		break;
	case ESoundChannel::SFX:
		TargetClass = SFXClass;
		CurrentSFXVolume = Volume;
		break;
	}

	if (TargetClass)
	{
		const float Pitch = 1.f;
		const float FadeInTime = 0.f;
		const bool bApplyToChildren = true;
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, TargetClass, Volume, Pitch, FadeInTime, bApplyToChildren);
	}
}

void USoundSubsystem::LoadVolumesFromSaveData()
{
	CurrentMasterVolume = 1.f;
	CurrentBGMVolume = 1.f;
	CurrentSFXVolume = 1.f;

	if(UGameplayStatics::DoesSaveGameExist(TEXT("OptionSaveSlot"), 0))
	{
		ULROptionSaveGame* LoadData = Cast<ULROptionSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("OptionSaveSlot"), 0));
		if(LoadData)
		{
			CurrentMasterVolume = LoadData->MasterVolume;
			CurrentBGMVolume = LoadData->BGMVolume;
			CurrentSFXVolume = LoadData->SFXVolume;
		}
	}

	ApplySoundMix();
}

void USoundSubsystem::SaveVolumesToSaveData()
{
	ULROptionSaveGame* SaveData = Cast<ULROptionSaveGame>(UGameplayStatics::CreateSaveGameObject(ULROptionSaveGame::StaticClass()));
	if (SaveData)
	{
		SaveData->MasterVolume = CurrentMasterVolume;
		SaveData->BGMVolume = CurrentBGMVolume;
		SaveData->SFXVolume = CurrentSFXVolume;

		UGameplayStatics::SaveGameToSlot(SaveData, TEXT("OptionSaveSlot"), 0);
	}
}

void USoundSubsystem::ApplySoundMix()
{
	SetVolume(ESoundChannel::Master, CurrentMasterVolume);
	SetVolume(ESoundChannel::BGM, CurrentBGMVolume);
	SetVolume(ESoundChannel::SFX, CurrentSFXVolume);
}
