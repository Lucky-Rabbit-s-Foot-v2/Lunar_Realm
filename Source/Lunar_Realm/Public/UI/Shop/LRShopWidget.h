// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRShopWidget.generated.h"


//============================================================================
/**
 * 상점 페이지 UI 위젯
 * - 기본적으로 존재하는 UI는 없으며, 추후 기획에 따라 구현 예정
 */
 //============================================================================
 // (260213) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRShopWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void CloseButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Close;
};
