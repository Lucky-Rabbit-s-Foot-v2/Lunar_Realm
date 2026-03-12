// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyViewWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

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

	SetIconByType();

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

void ULRCurrencyViewWidget::SetIconByType()
{
	LR_SCREEN_INFO(TEXT("Setting icon for currency type: %d"), static_cast<int32>(CurrencyType));
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	if (Img_Icon)
	{
		FName CurrencyID = TypeToID();
		LR_SCREEN_INFO(TEXT("Mapped currency type %d to CurrencyID: %s"), static_cast<int32>(CurrencyType), *CurrencyID.ToString());
		const FCurrencyStaticData& CurrencyData = GameDataSubsystem->GetCurrencyStaticData(CurrencyID);
		LR_SCREEN_INFO(TEXT("Retrieved static data for CurrencyID %s: Name=%s, Icon=%s"), *CurrencyID.ToString(), *CurrencyData.CurrencyImage.ToString(), *CurrencyData.CurrencyImage.GetAssetName());
		UTexture2D* IconTexture = CurrencyData.CurrencyImage.LoadSynchronous();
		LR_SCREEN_INFO(TEXT("Loaded icon texture for currency type %d: %s\n\n"), static_cast<int32>(CurrencyType), *GetNameSafe(IconTexture));
		Img_Icon->SetBrushFromTexture(IconTexture);
	}
}

FName ULRCurrencyViewWidget::TypeToID()
{
	switch (CurrencyType)
	{
	case ELRCurrencyType::Gold:
		return FName("GOLD");
	case ELRCurrencyType::CrescentTicket:
		return FName("CRESCENT");
	case ELRCurrencyType::FullMoonTicket:
		return FName("FULLMOON");
	}
	return FName();
}
