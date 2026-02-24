// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRIntroController.h"

#include "UI/Intro/LRIntroWidget.h"
#include "UI/Intro/LRTitleWidget.h"

void ALRIntroController::BeginPlay()
{
	Super::BeginPlay();
	
	SetCurrentPersistentType(EPersistentType::INTRO);
}

void ALRIntroController::OpenIntroWidget()
{
	if (IntroWidgetClass)
	{
		OpenWidget(IntroWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("IntroWidgetClass is not set in LRIntroController"));
	}
}

void ALRIntroController::OpenTitleWidget()
{
	if (TitleWidgetClass)
	{
		OpenWidget(TitleWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("TitleWidgetClass is not set in LRIntroController"));
	}
}