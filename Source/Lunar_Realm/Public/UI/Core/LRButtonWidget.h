// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRButtonWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLRButtonClicked);

UCLASS()
class LUNAR_REALM_API ULRButtonWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void BindProperties() override;

	FOnLRButtonClicked OnLRButtonClickedDel;

	UFUNCTION()
	virtual void OnButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Button")
	FText ButtonText;

	FTimerHandle ClickTimer;
	bool bCanClicked = true;
};
