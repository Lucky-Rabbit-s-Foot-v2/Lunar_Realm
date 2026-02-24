// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRCurrencyWidget.h"

#include "UI/Common/LRCurrencyViewWidget.h"

void ULRCurrencyWidget::RefreshUI()
{
	Super::RefreshUI();

	GoldView->RefreshUI();
	CrescentView->RefreshUI();
	FullMoonView->RefreshUI();
}
