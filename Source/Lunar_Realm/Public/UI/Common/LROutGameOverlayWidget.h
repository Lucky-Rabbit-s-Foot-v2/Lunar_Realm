// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LROverlayWidget.h"
#include "LROutGameOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULROutGameOverlayWidget : public ULROverlayWidget
{
	GENERATED_BODY()
	

public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void OnMessageButtonClicked();

	UFUNCTION()
	void OnSettingButtonClicked();


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Setting;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Menu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCurrencyWidget> Currency;
};
