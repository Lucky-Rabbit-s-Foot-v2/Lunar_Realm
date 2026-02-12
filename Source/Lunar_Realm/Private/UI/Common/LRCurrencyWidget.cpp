// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyWidget.h"

#include "System/LoggingSystem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "GameFramework/PlayerController.h"
#include "UI/HUD/LRLobbyHUD.h"

void ULRCurrencyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.AddDynamic(this, &ULRCurrencyWidget::OnCurrencyAddClicked);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ALRLobbyHUD* LobbyHUD = Cast<ALRLobbyHUD>(PC->GetHUD()))
		{
			OnCurrencyAddClickedDel.AddDynamic(LobbyHUD, &ALRLobbyHUD::OpenShopWidgetByCurrency);
		}
	}
}

void ULRCurrencyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRCurrencyWidget::OpenUI()
{
	Super::OpenUI();

	Super::RefreshUI();
}

void ULRCurrencyWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRCurrencyWidget::OnCurrencyAddClicked()
{
	// TODO: 재화 충전 UI 오픈
	OnCurrencyAddClickedDel.Broadcast();
}
