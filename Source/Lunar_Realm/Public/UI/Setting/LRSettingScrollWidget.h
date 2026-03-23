// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRSettingScrollWidget.generated.h"


// =============================================================================
/**
 * 설정창 내부 스크롤 위젯
 */
 //=============================================================================
 // (260219) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRSettingScrollWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	void SaveAllSettings();
	void SetDefaultSettings();

	virtual void RegisterSubWidgets() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameButtonsWidget> TextureWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameButtonsWidget> ShadowWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameButtonsWidget> AntiAliasingWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameButtonsWidget> PostProcessingWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameButtonsWidget> VFXWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameBarWidget> ResolutionScaleWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameBarWidget> FrameRateWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameBarWidget> MasterSoundWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameBarWidget> BGMWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULROptionNameBarWidget> SFXWidget;

};
