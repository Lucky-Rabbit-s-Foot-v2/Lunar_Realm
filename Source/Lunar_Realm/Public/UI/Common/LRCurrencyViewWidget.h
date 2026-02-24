// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/LREnumType.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRCurrencyViewWidget.generated.h"


// =============================================================================
/**
 * 재화 표시 위젯
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrencyAddClicked);

UCLASS()
class LUNAR_REALM_API ULRCurrencyViewWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RefreshUI() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|Currency")
	FOnCurrencyAddClicked OnCurrencyAddClickedDel;

private:
	UFUNCTION()
	void OnCurrencyAddClicked();

protected:
	// 재화 아이콘
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon = nullptr;
	// 재화 수량 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Amount = nullptr;
	// 재화 추가 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Add = nullptr;

private:
	UPROPERTY(EditAnywhere, Category = "LR|Currency")
	ELRCurrencyType CurrencyType = ELRCurrencyType::Gold;
};
