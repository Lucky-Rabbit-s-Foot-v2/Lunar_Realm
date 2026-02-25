// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/LRShopWidget.h"

#include "Components/Button.h"
#include "Units/LRControllerBase.h"

#include "UI/Lobby/LRLobbyWidget.h"

void ULRShopWidget::BindProperties()
{
	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ULRShopWidget::CloseButtonClicked);

	Super::BindProperties();
}

void ULRShopWidget::UnbindProperties()
{
	if (Btn_Close) Btn_Close->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRShopWidget::CloseButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::LOBBY);
}
