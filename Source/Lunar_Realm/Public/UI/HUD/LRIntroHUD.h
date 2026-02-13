// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseHUD.h"
#include "LRIntroHUD.generated.h"


// =============================================================================
/**
 * Intro 맵 전용 HUD 클래스
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ALRIntroHUD : public ABaseHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OpenIntroWidget();

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OpenTitleWidget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRIntroWidget> IntroWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRTitleWidget> TitleWidgetClass;
};
