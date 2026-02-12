// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyWidget.h"

#include "System/LoggingSystem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULRCurrencyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.AddDynamic(this, &ULRCurrencyWidget::OnCurrencyAddClicked);
	}
}

void ULRCurrencyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRCurrencyWidget::OpenUI()
{
	Super::OpenUI();

	Super::RefreshUI();
}

void ULRCurrencyWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRCurrencyWidget::OnCurrencyAddClicked()
{
	LR_SCREEN_INFO(TEXT("Currency Add Button Clicked: %d"), static_cast<uint8>(CurrencyType));
	OnCurrencyAddClickedDel.Broadcast();
}
