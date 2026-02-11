// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/LROptionDataStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GraphicSubsystem.generated.h"


//============================================================================
/**
 * 그래픽 세팅을 담당하는 서브시스템
 *
 * 주요 기능:
 * - 그래픽 처리
 */
//============================================================================
// (260204) PJB 제작. 제반 사항 구현.
// (260210) PJB 수정. 옵션 메니저 서브시스템과의 연동 고려하여 일부 기능 이동 및 수정.
//============================================================================

UENUM(BlueprintType)
enum class EGraphicOptionLevel : uint8
{
	Low			UMETA(DisplayName = "Low"),
	Medium		UMETA(DisplayName = "Medium"),
	High		UMETA(DisplayName = "High")
};

UCLASS()
class LUNAR_REALM_API UGraphicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	* 옵션 메니저에서 옵션 데이터 로드 시 호출.
	*/
	void InitializeFromSaveData(const FGraphicOptionData& NewLoadedOptions);

	/**
	* 옵션 적용 시 현재 옵션 정보를 전체 저장
	*/
	UFUNCTION(BlueprintCallable)
	void ApplyOptions();

	UFUNCTION(BlueprintCallable)
	FGraphicOptionData GetCurrentOptions() const { return CurrentOptions; }

	/**
	* 그래픽 세팅 함수
	* - 현재 옵션 정보에 저장 (반영되지 않음)
	*/
	UFUNCTION(BlueprintCallable)
	void SetTextureQuality(EGraphicOptionLevel Level) { CurrentOptions.TextureQuality = ConvertLevelToInt(Level); }
	
	UFUNCTION(BlueprintCallable)
	void SetShadowQuality(EGraphicOptionLevel Level) { CurrentOptions.ShadowQuality = ConvertLevelToInt(Level); }
	
	UFUNCTION(BlueprintCallable)
	void SetAntiAliasingQuality(EGraphicOptionLevel Level) { CurrentOptions.AntiAliasingQuality = ConvertLevelToInt(Level); }
	
	UFUNCTION(BlueprintCallable)
	void SetPostProcessingQuality(EGraphicOptionLevel Level) { CurrentOptions.PostProcessingQuality = ConvertLevelToInt(Level); }
	
	UFUNCTION(BlueprintCallable)
	void SetVisualEffectQuality(EGraphicOptionLevel Level) { CurrentOptions.VisualEffectQuality = ConvertLevelToInt(Level); }
	
	UFUNCTION(BlueprintCallable)
	void SetResolutionScale(float Percent) { CurrentOptions.ResolutionScale = Percent; }
	
	UFUNCTION(BlueprintCallable)
	void SetFrameRateLimit(float Limit) { CurrentOptions.FrameRateLimit = Limit; }

private:
	/**
	* 그래픽 옵션 레벨 헬퍼 함수
	*/
	int32 ConvertLevelToInt(EGraphicOptionLevel Level) const;
	
	FGraphicOptionData CurrentOptions;
};
