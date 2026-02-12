// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRLobbyHUD.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/OutGame/LRLobbyWidget.h"
#include "UI/Gacha/LRGachaShopWidget.h"

void ALRLobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	OpenLobbyWidget();
}

void ALRLobbyHUD::OpenLobbyWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	
	ULRLobbyWidget* LobbyWidget = UIManager->GetOrCreateWidget<ULRLobbyWidget>(LobbyWidgetClass);
	if (LobbyWidget && !LobbyWidget->IsOpen())
	{
		UIManager->OpenUI<ULRLobbyWidget>(LobbyWidgetClass);
	}
}

void ALRLobbyHUD::OpenShopWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	UBaseWidget* ShopWidget = UIManager->GetOrCreateWidget<UBaseWidget>(ShopWidgetClass);
	if (ShopWidget && !ShopWidget->IsOpen())
	{
		UIManager->OpenUI<UBaseWidget>(ShopWidgetClass);
	}
}

void ALRLobbyHUD::OpenShopWidgetByCurrency()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	UBaseWidget* ShopWidget = UIManager->GetOrCreateWidget<UBaseWidget>(ShopWidgetClass);
	if (ShopWidget && !ShopWidget->IsOpen())
	{
		UIManager->OpenUI<UBaseWidget>(ShopWidgetClass);
	}
}

void ALRLobbyHUD::OpenGachaShopWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	ULRGachaShopWidget* GachaShopWidget = UIManager->GetOrCreateWidget<ULRGachaShopWidget>(GachaShopWidgetClass);
	if (GachaShopWidget && !GachaShopWidget->IsOpen())
	{
		UIManager->OpenUI<ULRGachaShopWidget>(GachaShopWidgetClass);
	}
}

void ALRLobbyHUD::OpenCollectionWidget()
{
}

void ALRLobbyHUD::OpenPartyWidget()
{
}

void ALRLobbyHUD::OpenChapterWidget()
{
}

void ALRLobbyHUD::OpenStageWidget()
{
}

void ALRLobbyHUD::OpenSettingsWidget()
{
}

void ALRLobbyHUD::OpenFigureInfo(FName CharacterID)
{
}

void ALRLobbyHUD::CloseFigureInfo()
{
}
