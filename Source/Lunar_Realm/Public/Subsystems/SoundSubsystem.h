// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Components/AudioComponent.h"
#include "SoundSubsystem.generated.h"

/**
 * 음향을 담당하는 서브시스템
 *
 * 주요 기능:
 * - 콘텐츠 별 음향 조절
 */

//============================================================================
// (260204) PJB 제작. 제반 사항 구현.
//============================================================================

UENUM(BlueprintType)
enum class ESoundChannel : uint8
{
	Master,
	BGM,
	SFX
};

UCLASS()
class LUNAR_REALM_API USoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	* BGM 제어
	*/
	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* NewBGM, float FadeTime = 1.f);

	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeTime = 1.f);

	/**
	* SFX 제어
	*/
	UFUNCTION(BlueprintCallable)
	void PlaySFX_2D(USoundBase* Sound, float Volume = 1.f, float Pitch = 1.f);

	UFUNCTION(BlueprintCallable)
	void PlaySFX_AtLocation(USoundBase* Sound, FVector Location, float Volume = 1.f, float Pitch = 1.f);

	UFUNCTION(BlueprintCallable)
	void SetVolume(ESoundChannel Channel, float Volume);

protected:
	UPROPERTY()
	UAudioComponent* CurrentBGMComp;

	UPROPERTY(EditDefaultsOnly)
	USoundClass* MasterClass;
	
	UPROPERTY(EditDefaultsOnly)
	USoundClass* BGMClass;

	UPROPERTY(EditDefaultsOnly)
	USoundClass* SFXClass;

	UPROPERTY(EditDefaultsOnly)
	USoundMix* GlobalSoundMix;

private:
	float CurrentMasterVolume = 1.f;
	float CurrentBGMVolume = 1.f;
	float CurrentSFXVolume = 1.f;

	void LoadVolumesFromSaveData();
	void SaveVolumesToSaveData();
	void ApplySoundMix();
};
