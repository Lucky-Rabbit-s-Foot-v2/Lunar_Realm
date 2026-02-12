// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRLobbyHUD.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/OutGame/LRLobbyWidget.h"

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
