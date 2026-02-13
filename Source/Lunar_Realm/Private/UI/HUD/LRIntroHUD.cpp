// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRIntroHUD.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "System/LoggingSystem.h"

#include "UI/Intro/LRIntroWidget.h"
#include "UI/Intro/LRTitleWidget.h"

void ALRIntroHUD::BeginPlay()
{
	Super::BeginPlay();
	
	OpenIntroWidget();
}

void ALRIntroHUD::OpenIntroWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	ULRIntroWidget* IntroWidget = UIManager->GetOrCreateWidget<ULRIntroWidget>(IntroWidgetClass);
	if (IntroWidget)
	{
		IntroWidget->OnIntroAnimFinishedDel.AddDynamic(this, &ALRIntroHUD::OpenTitleWidget);
		UIManager->OpenUI<ULRIntroWidget>(IntroWidgetClass);
	}
}

void ALRIntroHUD::OpenTitleWidget()
{
	LR_SCREEN_INFO(TEXT("Open Title Widget"));
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	
	ULRTitleWidget* TitleWidget = UIManager->GetOrCreateWidget<ULRTitleWidget>(TitleWidgetClass);
	if (TitleWidget)
	{
		UIManager->OpenUI<ULRTitleWidget>(TitleWidgetClass);
	}
}
