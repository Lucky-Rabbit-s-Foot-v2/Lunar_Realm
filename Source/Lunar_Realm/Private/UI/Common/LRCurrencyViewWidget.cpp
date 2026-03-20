// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCurrencyViewWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCurrencyViewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UCurrencySubsystem* CurrencySubsystem = GetGameInstance()->GetSubsystem<UCurrencySubsystem>())
	{
		CurrencySubsystem->OnCurrencyChangedDel.RemoveAll(this);
		CurrencySubsystem->OnCurrencyChangedDel.AddUniqueDynamic(this, &ULRCurrencyViewWidget::RefreshOnChanged);
	}

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->OnSaveGameLoadedDel.RemoveAll(this);
		SaveGameSubsystem->OnSaveGameLoadedDel.AddUniqueDynamic(this, &ULRCurrencyViewWidget::RefreshOnLoaded);
	}
}

void ULRCurrencyViewWidget::NativeDestruct()
{
	if (UCurrencySubsystem* CurrencySubsystem = GetGameInstance()->GetSubsystem<UCurrencySubsystem>())
	{
		CurrencySubsystem->OnCurrencyChangedDel.RemoveAll(this);
	}

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->OnSaveGameLoadedDel.RemoveAll(this);
	}

	Super::NativeDestruct();
}

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

void ULRCurrencyViewWidget::RefreshOnLoaded(ULRSaveGame* SaveGame)
{
	RefreshUI();
}

void ULRCurrencyViewWidget::RefreshOnChanged()
{
	RefreshUI();
}

void ULRCurrencyViewWidget::OnCurrencyAddClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SHOP);
}

void ULRCurrencyViewWidget::SetIconByType()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	if (Img_Icon)
	{
		FName CurrencyID = TypeToID();
		const FCurrencyStaticData& CurrencyData = GameDataSubsystem->GetCurrencyStaticData(CurrencyID);
		UTexture2D* IconTexture = CurrencyData.CurrencyImage.LoadSynchronous();
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
