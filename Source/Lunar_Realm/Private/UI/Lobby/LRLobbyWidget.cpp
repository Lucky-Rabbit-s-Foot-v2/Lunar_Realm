// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyWidget.h"

#include "UI/Lobby/LRLobbyFigureWidget.h"
#include "UI/Common/LRCurrencyWidget.h"

void ULRLobbyWidget::RefreshUI()
{
	Super::RefreshUI();

	Currency->RefreshUI();
}
