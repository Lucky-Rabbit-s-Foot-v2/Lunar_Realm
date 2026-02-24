// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRTransitionController.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Common/LRPersistentWidget.h"
#include "UI/Intro/LRLoadingWidget.h"


void ALRTransitionController::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentPersistentType(EPersistentType::TRANSITION);
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
