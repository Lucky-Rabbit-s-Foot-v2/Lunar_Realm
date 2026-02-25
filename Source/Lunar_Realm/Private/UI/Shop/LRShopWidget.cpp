// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/LRShopWidget.h"

#include "Components/Button.h"
#include "Units/LRControllerBase.h"

#include "UI/Lobby/LRLobbyWidget.h"

void ULRShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &ULRShopWidget::CloseButtonClicked);
	}
}

void ULRShopWidget::NativeDestruct()
{
	if (Btn_Close)
	{
		Btn_Close->OnClicked.Clear();
	}
	Super::NativeDestruct();
}

void ULRShopWidget::CloseButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIPageID::Lobby);
}
