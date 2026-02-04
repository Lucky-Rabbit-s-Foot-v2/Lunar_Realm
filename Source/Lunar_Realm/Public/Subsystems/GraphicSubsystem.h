// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GraphicSubsystem.generated.h"


/**
 * 그래픽 세팅을 담당하는 서브시스템
 *
 * 주요 기능:
 * - 그래픽 처리
 */

//============================================================================
// (260204) PJB 제작. 제반 사항 구현.
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

	UFUNCTION(BlueprintCallable)
	void SetTextureQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void SetShadowQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void SetAntiAliasingQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void SetPostProcessingQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void SetVisualEffectQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void SetResolutionScale(float Percent);

	UFUNCTION(BlueprintCallable)
	void SetFrameRateLimit(float Limit);

	UFUNCTION(BlueprintCallable)
	void SetOverallQuality(EGraphicOptionLevel Level);

	UFUNCTION(BlueprintCallable)
	void ApplyAndSave();

	UFUNCTION(BlueprintCallable)
	void RunAutoBenchmark();

private:
	int32 ConvertLevelToInt(EGraphicOptionLevel Level) const;

};
