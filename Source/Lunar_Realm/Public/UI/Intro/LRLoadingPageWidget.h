// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRLoadingPageWidget.generated.h"

// =============================================================================
/**
 * 로딩화면 위젯
 * - 로딩하는 동안 진행률 표시
 */
 //=============================================================================
 // (260206) PJB 제작. 로딩 위젯 기반 구성.
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRLoadingPageWidget : public ULRPageWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

	/** 로딩 완료 시 호출되는 함수 */
	void FinishLoading();

	//(260325) BJM 추가
	void SetLoadingProgress(float InProgress);
private:
	void UpdateLoadingAnimation();


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar_Loading = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UThrobber> Throbber_Loading;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Gear;

protected:
	UPROPERTY(EditAnywhere, Category = "LR|Loading|Gear")
	int32 RotationPerSecond = 180;

	UPROPERTY(EditAnywhere, Category = "LR|Loading|Icon")
	float WobbleFrequency = 5.f;

	UPROPERTY(EditAnywhere, Category = "LR|Loading|Icon")
	int32 WobbleAngle = 10;

private:
	FTimerHandle LoadingTimerHandle;

	float CurrentRotation = 0.f;
	float AnimationTime = 0.f;

	float Progress = 0.f;
	float ElapsedTime = 0.f;
	const float TotalDuration = 5.f;

	float TimerInterval = 0.01f;
};
