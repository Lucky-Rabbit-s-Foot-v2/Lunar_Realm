// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRTitleWidget.generated.h"

// =============================================================================
/**
 * 타이틀 화면 위젯
 */
 //=============================================================================
 // (260206) PJB 제작. 타이틀 화면 위젯 기반 구성.
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRTitleWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
protected:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_BG;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Start;
};
