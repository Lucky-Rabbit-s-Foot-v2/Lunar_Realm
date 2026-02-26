// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyWidget.h"

#include "UI/Lobby/LRLobbyFigureWidget.h"
#include "UI/Common/LRCurrencyWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Components/Button.h"

void ULRLobbyWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Message->OnClicked.AddDynamic(this, &ULRLobbyWidget::OnMessageButtonClicked);
	Btn_Setting->OnClicked.AddDynamic(this, &ULRLobbyWidget::OnSettingButtonClicked);
}

void ULRLobbyWidget::UnbindProperties()
{
	Btn_Message->OnClicked.Clear();
	Btn_Setting->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRLobbyWidget::RefreshUI()
{
	Super::RefreshUI();

	Currency->RefreshUI();
}

void ULRLobbyWidget::OnMessageButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Message Button Clicked : Not implemented yet"));
}

void ULRLobbyWidget::OnSettingButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SETTING);
}
