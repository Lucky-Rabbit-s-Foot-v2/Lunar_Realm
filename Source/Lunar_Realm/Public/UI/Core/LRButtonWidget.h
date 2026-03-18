// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRButtonWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;

	UFUNCTION()
	virtual void OnButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image = nullptr;
};
