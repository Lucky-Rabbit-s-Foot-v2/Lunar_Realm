// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SoundSubsystem.h"

#include "Kismet/GameplayStatics.h"

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
		break;
	case ESoundChannel::BGM:
		TargetClass = BGMClass;
		break;
	case ESoundChannel::SFX:
		TargetClass = SFXClass;
		break;
	}

	float Pitch = 1.f;
	float FadeInTime = 0.f;
	bool bApplyToChildren = true;

	if (TargetClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, TargetClass, Volume, Pitch, FadeInTime, bApplyToChildren);
	}
}
