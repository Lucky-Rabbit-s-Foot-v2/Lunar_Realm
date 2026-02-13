// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRControllerBase.h"

void ALRControllerBase::CloseWidget(UBaseWidget* Widget)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->CloseUI(Widget);
}
