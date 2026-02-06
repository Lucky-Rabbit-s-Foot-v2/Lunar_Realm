// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRTitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRTitleWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRTitleWidget();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Start;
};
