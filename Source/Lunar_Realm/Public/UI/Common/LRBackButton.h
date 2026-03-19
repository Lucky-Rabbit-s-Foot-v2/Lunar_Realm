// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "LRBackButton.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRBackButton : public ULRButtonWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void OnButtonClicked() override;
	
	UFUNCTION()
	void SetVisibilityByUIHistory();
};
