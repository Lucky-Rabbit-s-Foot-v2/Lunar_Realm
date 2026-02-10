// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
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
class LUNAR_REALM_API ULRLoadingWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRLoadingWidget();

protected:
	virtual void NativeConstruct() override;

public:
	void FinishLoading();

private:
	void InitializeLoadingBar();
	void UpdateProgressBar();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar_Loading = nullptr;

private:
	FTimerHandle LoadingTimerHandle;

	float Progress = 0.f;
	float ElapsedTime = 0.f;
	const float TotalDuration = 7.f;
};
