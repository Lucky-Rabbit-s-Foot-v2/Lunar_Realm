// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyViewWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Shop/LRShopWidget.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCurrencyViewWidget::BindProperties()
{
	Super::BindProperties();
	
	if (Btn_Add) Btn_Add->OnClicked.AddDynamic(this, &ULRCurrencyViewWidget::OnCurrencyAddClicked);
}

void ULRCurrencyViewWidget::UnbindProperties()
{
	if (Btn_Add) Btn_Add->OnClicked.Clear();

	Super::UnbindProperties();
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
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::SHOP);
}
