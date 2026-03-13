// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LRTransitionController.generated.h"

/**
 * (260219) PJB 제작.
 */
UCLASS()
class LUNAR_REALM_API ALRTransitionController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:	
	virtual void OpenFirstWidget() override;

	void FinishLoading();

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Transition")
	TSubclassOf<class ULRLoadingPageWidget> LoadingWidgetClass;
};
