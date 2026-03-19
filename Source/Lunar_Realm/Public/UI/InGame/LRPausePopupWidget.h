// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRPausePopupWidget.generated.h"


//============================================================================
/**
 * 인게임 일시정지 UI 위젯
 */
 //============================================================================
 // (260226) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPausePopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION(BlueprintCallable)
	void OnRestartButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void OnResumeButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnSettingButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Setting;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Exit;
};
