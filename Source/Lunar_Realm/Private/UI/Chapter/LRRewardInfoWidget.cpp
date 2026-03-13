// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRRewardInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRRewardInfoWidget::RefreshUI()
{
	Super::RefreshUI();

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	FCurrencyStaticData CurrencyData;
	switch (CurrencyType)
	{
	case ELRCurrencyType::Gold:
		CurrencyData = GameDataSubsystem->GetCurrencyStaticData(FName("GOLD"));
		break;
	case ELRCurrencyType::CrescentTicket:
		CurrencyData = GameDataSubsystem->GetCurrencyStaticData(FName("CRESCENT"));
		break;
	case ELRCurrencyType::FullMoonTicket:
		CurrencyData = GameDataSubsystem->GetCurrencyStaticData(FName("FULLMOON"));
		break;
	default:
		break;
	}
	Img_Reward->SetBrushFromTexture(CurrencyData.CurrencyImage.LoadSynchronous());
	Txt_Amount->SetText(FText::AsNumber(RewardAmount));
}

void ULRRewardInfoWidget::SetRewardType(ELRCurrencyType InCurrencyType)
{
	CurrencyType = InCurrencyType;
	RefreshUI();
}

void ULRRewardInfoWidget::SetRewardAmount(const int32 InAmount)
{
	RewardAmount = InAmount;

	RefreshUI();
}
