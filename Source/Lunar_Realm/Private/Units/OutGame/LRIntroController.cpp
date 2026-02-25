// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRIntroController.h"

#include "UI/Intro/LRIntroWidget.h"

void ALRIntroController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUI<ULRIntroWidget>(IntroWidgetClass);
}
