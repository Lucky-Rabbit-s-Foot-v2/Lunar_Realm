// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRLobbyWidget.h"

#include "UI/OutGame/LRLobbyFigureWidget.h"
#include "UI/OutGame/LRCurrencyWidget.h"

void ULRLobbyWidget::RefreshUI()
{
	Super::RefreshUI();

	Currency->RefreshUI();
}
