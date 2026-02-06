// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRIntroHUD.h"

#include "UI/Intro/LRIntroWidget.h"

ALRIntroHUD::ALRIntroHUD()
{
}

void ALRIntroHUD::BeginPlay()
{
	Super::BeginPlay();

	if(IntroWidgetClass)
	{
		IntroWidget = CreateWidget<ULRIntroWidget>(GetWorld(), IntroWidgetClass);
		if(IntroWidget)
		{
			IntroWidget->AddToViewport();
			IntroWidget->PlayIntroAnimation();
		}
	}
}

void ALRIntroHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
