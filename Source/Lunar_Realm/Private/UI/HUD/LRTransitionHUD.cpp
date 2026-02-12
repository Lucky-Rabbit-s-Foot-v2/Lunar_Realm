// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRTransitionHUD.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Intro/LRLoadingWidget.h"

void ALRTransitionHUD::BeginPlay()
{
	Super::BeginPlay();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRLoadingWidget* LoadingWidget = UIManager->GetOrCreateWidget<ULRLoadingWidget>(LoadingWidgetClass);
	if (LoadingWidget)
	{
		UIManager->OpenUI<ULRLoadingWidget>(LoadingWidgetClass);
	}
}
