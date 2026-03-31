// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRGameClearPopupWidget.generated.h"


//============================================================================
/**
 * 게임 클리어 UI 위젯
 */
 //============================================================================
 // (260226) PJB 제작. 제반 사항 구현
 //============================================================================

class ULRExpPanelWidget;
class UImage;
class UTextBlock;


UCLASS()
class LUNAR_REALM_API ULRGameClearPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void RegisterSubWidgets() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void InitializeUI() override;

	void SetIsLastStage(bool bInIsLastStage);
	void SetStarMasking(int32 InMasking);

	UFUNCTION(BlueprintCallable)
	void OnNextStageButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_NextStage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStarBoxWidget> StarBox;

	bool bIsLastStage = false;

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* victory;

protected:
	UPROPERTY(meta = (BindWidget))
	ULRExpPanelWidget* LeaderExpPanel;

	UPROPERTY(meta = (BindWidget))
	ULRExpPanelWidget* MemberExpPanel;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	UFUNCTION()
	void ShowExpPanel();


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RewardGold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RewardNormalTicket;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RewardEnhanceTicket;

};
