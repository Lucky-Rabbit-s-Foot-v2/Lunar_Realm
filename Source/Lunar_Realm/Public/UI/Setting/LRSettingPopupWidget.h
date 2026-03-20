// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRSettingPopupWidget.generated.h"

// =============================================================================
/**
 * 설정창 위젯
 */
 //=============================================================================
 // (260219) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRSettingPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RegisterSubWidgets() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

private:
	UFUNCTION(BlueprintCallable)
	void OnCloseButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnDefaultButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnSaveButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Close;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Save;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Default;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSettingScrollWidget> SettingScrollWidget;

};
