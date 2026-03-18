// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyPageWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Lobby/LRLobbyFigureWidget.h"
#include "UI/Lobby/LRPageSelectorWidget.h"

void ULRLobbyPageWidget::RefreshUI()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	TArray<FName> PartyCharactersIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();

	MainCharacter->SetFigure(PartyCharactersIDs.IsValidIndex(0) ? PartyCharactersIDs[0] : NAME_None);
	Member1->SetFigure(PartyCharactersIDs.IsValidIndex(1) ? PartyCharactersIDs[1] : NAME_None);
	Member2->SetFigure(PartyCharactersIDs.IsValidIndex(2) ? PartyCharactersIDs[2] : NAME_None);
	Member3->SetFigure(PartyCharactersIDs.IsValidIndex(3) ? PartyCharactersIDs[3] : NAME_None);
	Member4->SetFigure(PartyCharactersIDs.IsValidIndex(4) ? PartyCharactersIDs[4] : NAME_None);
	
	Super::RefreshUI();
}

void ULRLobbyPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(MainCharacter);
	SubWidgets.Add(Member1);
	SubWidgets.Add(Member2);
	SubWidgets.Add(Member3);
	SubWidgets.Add(Member4);
	SubWidgets.Add(PageSelector);
}
