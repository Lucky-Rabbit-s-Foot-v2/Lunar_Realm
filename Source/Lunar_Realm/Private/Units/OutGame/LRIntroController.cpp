// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRIntroController.h"

#include "UI/Intro/LRIntroWidget.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"

void ALRIntroController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::INTRO);
}
