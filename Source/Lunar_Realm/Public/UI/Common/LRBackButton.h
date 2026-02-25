// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRBackButton.generated.h"


// =============================================================================
/**
 * 뒤로 가기 및 창 닫기 버튼
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRBackButton : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetParentWidget(ULRBaseWidget* InParentWidget) { ParentWidget = InParentWidget; }

	UFUNCTION()
	void OnBackButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock;

private:
	class ULRBaseWidget* ParentWidget;
};
