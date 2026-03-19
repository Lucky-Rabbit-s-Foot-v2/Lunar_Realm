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

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* victory;

};
