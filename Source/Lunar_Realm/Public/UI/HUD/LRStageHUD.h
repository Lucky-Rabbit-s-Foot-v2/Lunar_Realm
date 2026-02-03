// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LRStageHUD.generated.h"

/**
 * 
 */
class ULRPlayerWidget; 
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class LUNAR_REALM_API ALRStageHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// 오버레이 UI 초기화 함수
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<ULRPlayerWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<ULRPlayerWidget> OverlayWidget;
};
