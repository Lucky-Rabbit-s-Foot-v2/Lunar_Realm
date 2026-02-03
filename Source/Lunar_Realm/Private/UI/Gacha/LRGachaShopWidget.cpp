// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaShopWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "System/LoggingSystem.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

void ULRGachaShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GachaSys = GetGameInstance()->GetSubsystem<ULRGachaSubsystem>();

	// 기본 배너는 영웅으로 시작
	CurrentBannerID = DefaultHeroBannerID;

	if (ButtonHeroTab) ButtonHeroTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
	if (ButtonEquipTab) ButtonEquipTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);
	if (ButtonDraw1) ButtonDraw1->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickDraw1);
	if (ButtonDraw10) ButtonDraw10->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickDraw10);

	if (GachaSys)
	{
		// 재화/천장 변경 이벤트 구독
		GachaSys->OnCurrencyChanged.AddDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.AddDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	RefreshCurrencyTexts();
	RefreshPityText();
}

void ULRGachaShopWidget::NativeDestruct()
{
	if (GachaSys)
	{
		GachaSys->OnCurrencyChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	Super::NativeDestruct();
}

void ULRGachaShopWidget::OnClickHeroTab()
{
	CurrentBannerID = DefaultHeroBannerID;
	RefreshPityText();
}

void ULRGachaShopWidget::OnClickEquipTab()
{
	CurrentBannerID = DefaultEquipBannerID;
	RefreshPityText();
}

void ULRGachaShopWidget::OnClickDraw1()
{
	if (!GachaSys) return;

	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	const bool bOk = GachaSys->BeginDrawTransaction(CurrentBannerID, 1, TxnId, Results);
	if (!bOk)
	{
		LR_WARN(TEXT("BeginDrawTransaction(1) failed"));
		return;
	}

	// Reveal 팝업 열기
	if (RevealWidgetClass)
	{
		UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		if (UISys)
		{
			ULRGachaRevealWidget* Reveal = UISys->OpenUI<ULRGachaRevealWidget>(RevealWidgetClass);
			if (Reveal)
			{
				Reveal->StartRevealWithTransaction(CurrentBannerID, TxnId, Results);
			}
		}
	}
}

void ULRGachaShopWidget::OnClickDraw10()
{
	if (!GachaSys) return;

	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	const bool bOk = GachaSys->BeginDrawTransaction(CurrentBannerID, 10, TxnId, Results);
	if (!bOk)
	{
		LR_WARN(TEXT("BeginDrawTransaction(10) failed"));
		return;
	}

	if (RevealWidgetClass)
	{
		UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		if (UISys)
		{
			ULRGachaRevealWidget* Reveal = UISys->OpenUI<ULRGachaRevealWidget>(RevealWidgetClass);
			if (Reveal)
			{
				Reveal->StartRevealWithTransaction(CurrentBannerID, TxnId, Results);
			}
		}
	}
}

void ULRGachaShopWidget::HandleCurrencyChanged(FGameplayTag Tag, int32 NewValue)
{
	// 어떤 재화가 바뀌든 전체 텍스트를 갱신
	RefreshCurrencyTexts();
}

void ULRGachaShopWidget::HandlePityChanged(FName BannerID, int32 NewValue)
{
	if (BannerID == CurrentBannerID)
	{
		RefreshPityText();
	}
}

void ULRGachaShopWidget::RefreshCurrencyTexts()
{
	if (!GachaSys) return;

	const int32 Gold = GachaSys->GetCurrency(FGameplayTag::RequestGameplayTag(TEXT("Currency.Gold")));
	const int32 Crescent = GachaSys->GetCurrency(FGameplayTag::RequestGameplayTag(TEXT("Currency.Ticket.Crescent")));
	const int32 FullMoon = GachaSys->GetCurrency(FGameplayTag::RequestGameplayTag(TEXT("Currency.Ticket.FullMoon")));

	if (TextGold) TextGold->SetText(FText::AsNumber(Gold));
	if (TextCrescent) TextCrescent->SetText(FText::AsNumber(Crescent));
	if (TextFullMoon) TextFullMoon->SetText(FText::AsNumber(FullMoon));
}

void ULRGachaShopWidget::RefreshPityText()
{
	if (!GachaSys || !TextPity) return;

	// 보름달 배너만 천장 표시할거면, 여기서 BannerRow를 조회해서 bUsePity일 때만 표시하도록 블루프린트에서 처리 가능
	const int32 Pity = GachaSys->GetPityCount(CurrentBannerID);
	TextPity->SetText(FText::FromString(FString::Printf(TEXT("천장 카운트 : %d"), Pity)));
}
