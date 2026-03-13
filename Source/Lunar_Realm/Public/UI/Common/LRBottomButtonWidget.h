// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "LRBottomButtonWidget.generated.h"


//============================================================================
/**
 * 로비화면 하단에 위치한 버튼 위젯 클래스
 */
 //============================================================================
 // (260304) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRBottomButtonWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION()
	FEventReply OnBackgroundMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Background = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name = nullptr;

	UPROPERTY(EditAnywhere, Category = "LR|UI Button")
	EUIID MovePageUIID = EUIID::NONE;
};
