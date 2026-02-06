// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRStageHUD.h"
#include "UI/InGame/LRPlayerWidget.h"

void ALRStageHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_LRHUD"));

	OverlayWidget = CreateWidget<ULRPlayerWidget>(GetWorld(), OverlayWidgetClass);

	OverlayWidget->OpenUI();
	OverlayWidget->AddToViewport(); 

	OverlayWidget->SetWidgetController(PC);
}
