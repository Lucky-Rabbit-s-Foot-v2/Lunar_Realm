// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyPageWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Lobby/LRLobbyFigureWidget.h"
#include "UI/Lobby/LRLobbyFigureInfoWidget.h"

#include "UI/Lobby/LRPageSelectorWidget.h"

#include "TimerManager.h"

#include "Units/OutGame/LROutGameController.h"


void ULRLobbyPageWidget::OpenUI()
{
	Super::OpenUI();

	MainCharacter->SetSlotIndex(0);
	Member1->SetSlotIndex(1);
	Member2->SetSlotIndex(2);
	Member3->SetSlotIndex(3);
	Member4->SetSlotIndex(4);

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
