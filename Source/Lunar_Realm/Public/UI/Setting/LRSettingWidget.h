// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRSettingWidget.generated.h"

// =============================================================================
/**
 * 설정창 위젯
 */
 //=============================================================================
 // (260219) PJB 제작.
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDefaultButtonClicked);

UCLASS()
class LUNAR_REALM_API ULRSettingWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FOnCloseButtonClicked OnCloseButtonClickedDel;
	FOnDefaultButtonClicked OnDefaultButtonClickedDel;

private:
	UFUNCTION(BlueprintCallable)
	void OnCloseButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnDefaultButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Default;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSettingScrollWidget> SettingScrollWidget;

};
