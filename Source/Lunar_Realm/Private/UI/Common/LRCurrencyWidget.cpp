// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyWidget.h"

#include "System/LoggingSystem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCurrencyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.AddDynamic(this, &ULRCurrencyWidget::OnCurrencyAddClicked);
	}

	if (ALROutGameController* LRController = GetWorld()->GetFirstPlayerController<ALROutGameController>())
	{
		OnCurrencyAddClickedDel.AddDynamic(LRController, &ALROutGameController::OpenShopWidgetByCurrency);
	}
}

void ULRCurrencyWidget::NativeDestruct()
{
	OnCurrencyAddClickedDel.Clear();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.Clear();
	}

	Super::NativeDestruct();
}

void ULRCurrencyWidget::OnCurrencyAddClicked()
{
	OnCurrencyAddClickedDel.Broadcast();
}
