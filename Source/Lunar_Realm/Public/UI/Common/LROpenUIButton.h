// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "LROpenUIButton.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULROpenUIButton : public ULRButtonWidget
{
	GENERATED_BODY()
	
public:
	virtual void OnButtonClicked() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI|Open Button")
	EUIID TargetUIID = EUIID::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI|Open Button")
	TSubclassOf<ULRBaseWidget> TargetUIClass;
};
