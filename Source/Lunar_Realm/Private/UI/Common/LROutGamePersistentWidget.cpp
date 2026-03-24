// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LROutGamePersistentWidget.h"

#include "UI/Common/LRCurrencyWidget.h"

void ULROutGamePersistentWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	SubWidgets.Add(Currency);
}
