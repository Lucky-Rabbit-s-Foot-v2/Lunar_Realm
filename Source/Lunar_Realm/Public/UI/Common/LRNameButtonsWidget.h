// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPopupWidget.h"
#include "LRNameButtonsWidget.generated.h"


// =============================================================================
/**
 * 이름과 버튼 3개만 존재하는 간단한 UI 위젯
 */
 //=============================================================================
 // (260219) PJB 제작. 제반 사항 구현.
 // =============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, int32, Number);

UCLASS()
class LUNAR_REALM_API ULRNameButtonsWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FOnButtonClicked OnButtonClickedDel;

	void SetName(const FText& Name);
private:
	UFUNCTION(BlueprintCallable)
	void OnLowButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnMediumButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnHighButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Low;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Medium;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_High;
};
