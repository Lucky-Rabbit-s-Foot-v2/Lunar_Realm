// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/Option/SoundSubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/LROptionSaveGame.h"
#include "Subsystems/Option/OptionManagerSubsystem.h"

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GlobalSoundMix)
	{
		UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	}
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
		CurrentOptions.MasterVolume = Volume;
		break;
	case ESoundChannel::BGM:
		TargetClass = BGMClass;
		CurrentOptions.BGMVolume = Volume;
		break;
	case ESoundChannel::SFX:
		TargetClass = SFXClass;
		CurrentOptions.SFXVolume = Volume;
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

void USoundSubsystem::InitializeFromSaveData(const FSoundOptionData& Data)
{
	CurrentOptions = Data;
	ApplyOptions();
}

void USoundSubsystem::ApplyOptions()
{
	SetVolume(ESoundChannel::Master, CurrentOptions.MasterVolume);
	SetVolume(ESoundChannel::BGM, CurrentOptions.BGMVolume);
	SetVolume(ESoundChannel::SFX, CurrentOptions.SFXVolume);
}
