// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "LREnhanceButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULREnhanceButtonWidget : public ULRButtonWidget
{
	GENERATED_BODY()
	

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Cost = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Enhance")
	int32 EnhanceCount = 0;
};
