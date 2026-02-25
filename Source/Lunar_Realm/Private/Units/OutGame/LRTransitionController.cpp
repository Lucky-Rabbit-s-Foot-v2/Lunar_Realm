// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRTransitionController.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "UI/Core/LRPersistentWidget.h"
#include "UI/Intro/LRLoadingWidget.h"

void ALRTransitionController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::LOADING);
}

void ALRTransitionController::FinishLoading()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (LoadingWidgetClass)
	{
		ULRLoadingWidget* LoadingWidgetInstance = UIManager->GetOrCreateWidget<ULRLoadingWidget>(LoadingWidgetClass);
		LoadingWidgetInstance->FinishLoading();
	}
	else
	{
		LR_WARN(TEXT("LoadingWidgetClass is not set in LRTransitionGameMode"));
	}
}
