// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyWidget.h"

#include "UI/Common/LRCurrencyViewWidget.h"

void ULRCurrencyWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	
	SubWidgets.Add(GoldView);
	SubWidgets.Add(CrescentView);
	SubWidgets.Add(FullMoonView);
}
