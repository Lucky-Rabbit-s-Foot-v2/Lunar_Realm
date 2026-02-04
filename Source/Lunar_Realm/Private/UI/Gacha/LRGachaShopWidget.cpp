// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaShopWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "System/LoggingSystem.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogLRGachaShop, Log, All);

void ULRGachaShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GachaSys = GetGameInstance()->GetSubsystem<ULRGachaSubsystem>();

	// 기본 배너는 영웅으로 시작
	CurrentBannerID = DefaultHeroBannerID;

	if (ButtonHeroTab) ButtonHeroTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
	if (ButtonEquipTab) ButtonEquipTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);

	if (ButtonCrescentDraw1)  ButtonCrescentDraw1->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw1);
	if (ButtonCrescentDraw10) ButtonCrescentDraw10->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw10);
	if (ButtonFullMoonDraw1)  ButtonFullMoonDraw1->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw1);
	if (ButtonFullMoonDraw10) ButtonFullMoonDraw10->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw10);


	if (GachaSys)
	{
		// 재화/천장 변경 이벤트 구독
		GachaSys->OnCurrencyChanged.AddDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.AddDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	RefreshCurrencyTexts();
	RefreshPityText();

	if (GachaSys)
	{
		FLRGachaPendingTransaction Pending;
		if (GachaSys->GetAnyPendingTransaction(Pending))
		{
			// Pending 안에 BannerID, TxnId, Results가 있다고 가정
			ShowRevealWidget(Pending.BannerID, Pending.TxnId, Pending.Results);
			return;
		}
	}
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

// 리빌 위젯 띄우기
void ULRGachaShopWidget::ShowRevealWidget(FName InBannerID, const FGuid& InTxnId, const TArray<FLRGachaResult>& InResults)
{
	if (!RevealWidgetClass) return;

	UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (!UISys) return;

	// UIManager로 열어서 Popup 스택/입력모드까지 정상 처리
	ULRGachaRevealWidget* Reveal = UISys->OpenUI<ULRGachaRevealWidget>(RevealWidgetClass);
	if (!Reveal) return;

	Reveal->StartRevealWithTransaction(InBannerID, InTxnId, InResults);
}

// Draw 버튼 클릭에서 트랜잭션 시작 + 리빌 띄우기
void ULRGachaShopWidget::TryBeginDrawAndOpenReveal(FName BannerID, int32 Count)
{
	if (!GachaSys) return;

	RefreshPityText();

	int32 NeedCost = 0;
	if (!GachaSys->CanDraw(BannerID, Count, NeedCost))
	{
		// TODO: 토스트/팝업으로 “재화 부족” 표시하면 UX 좋아짐
		UE_LOG(LogLRGachaShop, Warning, TEXT("[GachaShop] CanDraw Failed. Banner=%s Count=%d NeedCost=%d"),
			*BannerID.ToString(), Count, NeedCost);
		return;
	}

	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	const bool bOk = GachaSys->BeginDrawTransaction(BannerID, Count, TxnId, Results);
	if (!bOk || !TxnId.IsValid() || Results.Num() == 0)
	{
		UE_LOG(LogLRGachaShop, Error, TEXT("[GachaShop] BeginDrawTransaction Failed. Banner=%s Count=%d"),
			*BannerID.ToString(), Count);
		return;
	}

	// 디버그 로그
	UE_LOG(LogLRGachaShop, Log, TEXT("[GachaShop] Txn Started. Banner=%s Count=%d Txn=%s Results=%d"),
		*BannerID.ToString(), Count, *TxnId.ToString(), Results.Num());

	ShowRevealWidget(BannerID, TxnId, Results);
}

void ULRGachaShopWidget::OnClickHeroTab()
{
	CurrentBannerID = DefaultHeroBannerID; // Hero_FullMoon 기본
	RefreshPityText();
}

void ULRGachaShopWidget::OnClickEquipTab()
{
	CurrentBannerID = DefaultEquipBannerID; // Equip_FullMoon 기본
	RefreshPityText();
}

void ULRGachaShopWidget::OnClickCrescentDraw1()
{
	const FName BannerID = MakeBannerIDForTicket(false);
	TryBeginDrawAndOpenReveal(BannerID, 1);
}

void ULRGachaShopWidget::OnClickCrescentDraw10()
{
	const FName BannerID = MakeBannerIDForTicket(false);
	TryBeginDrawAndOpenReveal(BannerID, 10);
}

void ULRGachaShopWidget::OnClickFullMoonDraw1()
{
	const FName BannerID = MakeBannerIDForTicket(true);
	TryBeginDrawAndOpenReveal(BannerID, 1);
}

void ULRGachaShopWidget::OnClickFullMoonDraw10()
{
	const FName BannerID = MakeBannerIDForTicket(true);
	TryBeginDrawAndOpenReveal(BannerID, 10);
}


void ULRGachaShopWidget::HandleCurrencyChanged(FGameplayTag Tag, int32 NewValue)
{
	// 어떤 재화가 바뀌든 전체 텍스트를 갱신
	RefreshCurrencyTexts();
}

void ULRGachaShopWidget::HandlePityChanged(FName BannerID, int32 NewValue)
{
	// 표시 천장은 FullMoon만 본다
	const FName DisplayBannerID = IsHeroTabSelected() ? FName(TEXT("Hero_FullMoon")) : FName(TEXT("Equip_FullMoon"));

	if (BannerID == DisplayBannerID)
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

	const ELRGachaItemType ItemType = IsHeroTabSelected() ? ELRGachaItemType::Hero : ELRGachaItemType::Equipment;
	const int32 Pity = GachaSys->GetDisplayPityCount(ItemType);

	TextPity->SetText(FText::FromString(FString::Printf(TEXT("천장 카운트 : %d"), Pity)));
}

bool ULRGachaShopWidget::IsHeroTabSelected() const
{
	// 현재 배너가 Hero로 시작하면 영웅 탭으로 판단(네 네이밍 규칙 기반)
	const FString S = CurrentBannerID.ToString();
	return S.StartsWith(TEXT("Hero_"));
}

FName ULRGachaShopWidget::MakeBannerIDForTicket(bool bFullMoon) const
{
	// 네 배너 네이밍 규칙이 Hero_FullMoon / Hero_Crescent 이런 형태라는 전제
	const bool bHero = IsHeroTabSelected();

	if (bHero)
	{
		return bFullMoon ? FName(TEXT("Hero_FullMoon")) : FName(TEXT("Hero_Crescent"));
	}
	else
	{
		return bFullMoon ? FName(TEXT("Equip_FullMoon")) : FName(TEXT("Equip_Crescent"));
	}
}
