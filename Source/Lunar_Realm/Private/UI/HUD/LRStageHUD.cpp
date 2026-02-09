// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRStageHUD.h"
#include "UI/InGame/LRPlayerWidget.h"

void ALRStageHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_LRHUD"));

	OverlayWidget = CreateWidget<ULRPlayerWidget>(PC, OverlayWidgetClass);

	OverlayWidget->OpenUI();
	OverlayWidget->AddToViewport(); 

	OverlayWidget->SetWidgetController(PC);

	if (ASC)
	{
		OverlayWidget->InitializeGAS(ASC);
		UE_LOG(LogTemp, Warning, TEXT("[HUD] 위젯에게 ASC 직접 전달 완료!"));
	}

	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = true;
	}

}
