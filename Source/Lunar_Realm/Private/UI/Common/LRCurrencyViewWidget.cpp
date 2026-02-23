// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyViewWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCurrencyViewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.AddDynamic(this, &ULRCurrencyViewWidget::OnCurrencyAddClicked);
	}

	if (ALROutGameController* LRController = GetWorld()->GetFirstPlayerController<ALROutGameController>())
	{
		OnCurrencyAddClickedDel.AddDynamic(LRController, &ALROutGameController::OpenShopWidgetByCurrency);
	}
}

void ULRCurrencyViewWidget::NativeDestruct()
{
	OnCurrencyAddClickedDel.Clear();

	if (Btn_Add)
	{
		Btn_Add->OnClicked.Clear();
	}

	Super::NativeDestruct();
}

void ULRCurrencyViewWidget::RefreshUI()
{
	Super::RefreshUI();

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	if (SaveGameSubsystem)
	{
		int32 CurrencyAmount = SaveGameSubsystem->GetCurrency(CurrencyType);
		if (Txt_Amount)
		{
			Txt_Amount->SetText(FText::AsNumber(CurrencyAmount));
		}
	}
}

void ULRCurrencyViewWidget::OnCurrencyAddClicked()
{
	OnCurrencyAddClickedDel.Broadcast();
}
