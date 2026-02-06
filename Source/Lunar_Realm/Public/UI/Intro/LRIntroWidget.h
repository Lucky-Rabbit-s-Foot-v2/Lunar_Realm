// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRIntroWidget.generated.h"

/**
 * (260206 PJB) 인트로 위젯 기반 구성
 */
UCLASS()
class LUNAR_REALM_API ULRIntroWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRIntroWidget();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void PlayIntroAnimation();

protected:
	UFUNCTION()
	void OnFinishedIntroAnim();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_3;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FadeAnim;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRTitleWidget> TitleWidgetClass;

};
