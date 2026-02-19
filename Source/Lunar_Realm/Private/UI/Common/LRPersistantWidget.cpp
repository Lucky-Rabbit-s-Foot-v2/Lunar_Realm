// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRPersistantWidget.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Common/LRPopupWidget.h"

void ULRPersistantWidget::InitializeUI()
{
	Super::InitializeUI();
	UILayer = EUILayer::PERSISTENT;
}

void ULRPersistantWidget::OpenUI()
{
	Super::OpenUI();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRPopupWidget* DefaultPopupWidget = UIManager->GetOrCreateWidget<ULRPopupWidget>(DefaultPopupClass);
	if (DefaultPopupWidget && !DefaultPopupWidget->IsOpen())
	{
		UIManager->OpenUI<ULRPopupWidget>(DefaultPopupClass);
	}
}