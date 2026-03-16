// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRTransitionController.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "UI/Intro/LRLoadingPageWidget.h"

void ALRTransitionController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::LOADING);
}

void ALRTransitionController::FinishLoading()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRLoadingPageWidget* LoadingWidget = Cast<ULRLoadingPageWidget>(UIManager->GetOrCreateWidgetByID(EUIID::LOADING));
	if (LoadingWidget)
	{
		LoadingWidget->FinishLoading();
	}
}
