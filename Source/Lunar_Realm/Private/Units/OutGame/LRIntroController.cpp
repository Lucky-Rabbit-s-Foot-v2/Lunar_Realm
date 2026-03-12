// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LRIntroController.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"

#include "UI/Core/LRBackgroundWidget.h"

#include "UI/Intro/LRIntroWidget.h"

void ALRIntroController::OpenFirstWidget()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::INTRO);
	}
}

void ALRIntroController::OpenTitleScreen()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::TITLE);
	}
}
