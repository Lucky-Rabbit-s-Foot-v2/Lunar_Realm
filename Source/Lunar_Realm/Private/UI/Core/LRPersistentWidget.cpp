// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRPersistentWidget.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Core/LRPopupWidget.h"

void ULRPersistentWidget::InitializeUI()
{
	Super::InitializeUI();
	UILayer = EUILayer::PERSISTENT;
}

void ULRPersistentWidget::OpenUI()
{
	Super::OpenUI();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	
	if (DefaultPopupClass)
	{
		UIManager->OpenUI<ULRPopupWidget>(DefaultPopupClass);
	}
}