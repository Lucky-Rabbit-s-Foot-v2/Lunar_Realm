// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRGameOverPopupWidget.generated.h"


//============================================================================
/**
 * 게임 오버	UI 위젯
 */
 //============================================================================
 // (260226) PJB 제작. 제반 사항 구현
 //============================================================================


class UImage;
class UTextBlock;

UCLASS()
class LUNAR_REALM_API ULRGameOverPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION(BlueprintCallable)
	void OnRegroupButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void OnRestartButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	virtual void InitializeUI() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Regroup;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Exit;

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Fail;

protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	UFUNCTION()
	void ShowExpPanel2();

	UPROPERTY(meta = (BindWidgetOptional))
	class ULRExpPanelWidget* LeaderExpPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	class ULRExpPanelWidget* MemberExpPanel;

};
