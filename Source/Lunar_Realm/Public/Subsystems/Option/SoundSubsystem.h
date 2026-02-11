// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/LROptionDataStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Components/AudioComponent.h"
#include "SoundSubsystem.generated.h"

//============================================================================
/**
 * 음향을 담당하는 서브시스템
 *
 * 주요 기능:
 * - 콘텐츠 별 음향 조절
 */
//============================================================================
// (260204) PJB 제작. 제반 사항 구현.
// (260210) PJB 수정. 옵션 메니저 서브시스템과의 연동 고려하여 일부 기능 이동 및 수정.
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
	
	/**
	* 데이터 로드 및 저장 함수
	*/
	void InitializeFromSaveData(const FSoundOptionData& Data);

	/**
	* 설정 정보 게임에 반영.
	*/
	UFUNCTION(BlueprintCallable)
	void ApplyOptions();

	UFUNCTION(BlueprintCallable)
	FSoundOptionData GetCurrentOptions() const { return CurrentOptions; }

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
	FSoundOptionData CurrentOptions;
};
