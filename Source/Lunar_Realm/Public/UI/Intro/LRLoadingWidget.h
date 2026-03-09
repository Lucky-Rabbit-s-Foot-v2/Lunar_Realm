// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRLoadingWidget.generated.h"

// =============================================================================
/**
 * 로딩화면 위젯
 * - 로딩하는 동안 진행률 표시
 */
 //=============================================================================
 // (260206) PJB 제작. 로딩 위젯 기반 구성.
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRLoadingWidget : public ULRPageWidget
{
	GENERATED_BODY()

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

	/** 로딩 완료 시 호출되는 함수 */
	void FinishLoading();

private:
	void UpdateProgressBar();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar_Loading = nullptr;

private:
	FTimerHandle LoadingTimerHandle;

	float Progress = 0.f;
	float ElapsedTime = 0.f;
	const float TotalDuration = 5.f;
};
