// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRIntroController.h"

#include "UI/Intro/LRIntroWidget.h"
#include "UI/Intro/LRTitleWidget.h"

void ALRIntroController::BeginPlay()
{
	Super::BeginPlay();
	
	OpenIntroWidget();
}

void ALRIntroController::OpenIntroWidget()
{
	ULRIntroWidget* IntroWidget = OpenWidget(IntroWidgetClass);
	if (IntroWidget)
	{
		IntroWidget->OnIntroAnimFinishedDel.AddDynamic(this, &ALRIntroController::OpenTitleWidget);
	}
}

void ALRIntroController::OpenTitleWidget()
{
	OpenWidget(TitleWidgetClass);
}