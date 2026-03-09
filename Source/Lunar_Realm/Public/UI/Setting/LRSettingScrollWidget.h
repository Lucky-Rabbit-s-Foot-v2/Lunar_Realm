// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRSettingScrollWidget.generated.h"


// =============================================================================
/**
 * 설정창 내부 스크롤 위젯
 */
 //=============================================================================
 // (260219) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRSettingScrollWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;

	void SetDefaultSettings();

	void SaveAllSettings();

	virtual void RegisterSubWidgets() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameButtonsWidget> TextureWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameButtonsWidget> ShadowWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameButtonsWidget> AntiAliasingWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameButtonsWidget> PostProcessingWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameButtonsWidget> VFXWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameBarWidget> ResolutionScaleWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameBarWidget> FrameRateWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameBarWidget> MasterSoundWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameBarWidget> BGMWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRNameBarWidget> SFXWidget;

};
