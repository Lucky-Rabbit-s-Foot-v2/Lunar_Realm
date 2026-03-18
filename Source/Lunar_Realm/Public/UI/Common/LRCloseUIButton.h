// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "LRCloseUIButton.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCloseUIButton : public ULRButtonWidget
{
	GENERATED_BODY()
	
public:
	void SetTargetWidget(ULRBaseWidget* InTargetWidget) { TargetWidget = InTargetWidget; }
	
	virtual void OnButtonClicked() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI|Close Button")
	class ULRBaseWidget* TargetWidget = nullptr;
};
