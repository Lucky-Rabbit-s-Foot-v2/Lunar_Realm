// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Lobby/LRLobbyFigureWidget.h"
#include "UI/Common/LRCurrencyWidget.h"

void ULRLobbyWidget::RefreshUI()
{
	Super::RefreshUI();

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	TArray<FName> PartyCharactersIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
	
	MainCharacter->SetFigure(PartyCharactersIDs.IsValidIndex(0) ? PartyCharactersIDs[0] : NAME_None);
	Member1->SetFigure(PartyCharactersIDs.IsValidIndex(1) ? PartyCharactersIDs[1] : NAME_None);
	Member2->SetFigure(PartyCharactersIDs.IsValidIndex(2) ? PartyCharactersIDs[2] : NAME_None);
	Member3->SetFigure(PartyCharactersIDs.IsValidIndex(3) ? PartyCharactersIDs[3] : NAME_None);
	Member4->SetFigure(PartyCharactersIDs.IsValidIndex(4) ? PartyCharactersIDs[4] : NAME_None);
}

void ULRLobbyWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Message) Btn_Message->OnClicked.AddDynamic(this, &ULRLobbyWidget::OnMessageButtonClicked);
	if (Btn_Setting) Btn_Setting->OnClicked.AddDynamic(this, &ULRLobbyWidget::OnSettingButtonClicked);
}

void ULRLobbyWidget::UnbindProperties()
{
	Btn_Message->OnClicked.Clear();
	Btn_Setting->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRLobbyWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(MainCharacter);
	SubWidgets.Add(Member1);
	SubWidgets.Add(Member2);
	SubWidgets.Add(Member3);
	SubWidgets.Add(Member4);
	SubWidgets.Add(Currency);
	SubWidgets.Add(Banner);
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
