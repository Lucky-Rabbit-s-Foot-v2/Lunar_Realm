// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LRIntroHUD.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRIntroHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ALRIntroHUD();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRIntroWidget> IntroWidgetClass = nullptr;

private:
	UPROPERTY()
	TObjectPtr<class ULRIntroWidget> IntroWidget = nullptr;
};
