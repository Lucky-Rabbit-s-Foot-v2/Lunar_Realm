// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaShopWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogLRGachaShop, Log, All);

void ULRGachaShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ───────────────── Subsystem 참조 ─────────────────
	if (UGameInstance* GI = GetGameInstance())
	{
		GachaSys = GI->GetSubsystem<ULRGachaSubsystem>();
	}

	// 기본 배너는 영웅으로 시작
	CurrentBannerID = DefaultHeroBannerID;

	// ───────────────── 버튼 바인딩 ─────────────────
	if (ButtonHeroTab)
	{
		ButtonHeroTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
	}
	if (ButtonEquipTab)
	{
		ButtonEquipTab->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);
	}

	if (ButtonCrescentDraw1)
	{
		ButtonCrescentDraw1->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw1);
	}
	if (ButtonCrescentDraw10)
	{
		ButtonCrescentDraw10->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw10);
	}
	if (ButtonFullMoonDraw1)
	{
		ButtonFullMoonDraw1->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw1);
	}
	if (ButtonFullMoonDraw10)
	{
		ButtonFullMoonDraw10->OnClicked.AddDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw10);
	}

	// ───────────────── 델리게이트 구독 ─────────────────
	// 재화/천장 변경 시 UI 자동 갱신
	if (GachaSys)
	{
		GachaSys->OnCurrencyChanged.AddDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.AddDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	// 초기 UI 갱신
	RefreshCurrencyTexts();
	RefreshPityText();

	// ───────────────── 튕김 복구 처리 ─────────────────
	// Pending 트랜잭션이 남아있으면 "자동 커밋" 후 리빌을 다시 보여줌.
	if (GachaSys)
	{
		FLRGachaPendingTransaction Pending;
		if (GachaSys->GetAnyPendingTransaction(Pending))
		{
			UE_LOG(
				LogLRGachaShop,
				Warning,
				TEXT("[GachaShop] Pending Txn found on enter. Auto-committing. Banner=%s Txn=%s Results=%d"),
				*Pending.BannerID.ToString(),
				*Pending.TxnId.ToString(),
				Pending.Results.Num()
			);

			const bool bCommitted = GachaSys->CommitTransaction(Pending.TxnId);
			if (!bCommitted)
			{
				UE_LOG(
					LogLRGachaShop,
					Error,
					TEXT("[GachaShop] Auto-commit failed. Banner=%s Txn=%s"),
					*Pending.BannerID.ToString(),
					*Pending.TxnId.ToString()
				);
				return;
			}

			// 새 플로우 - 리빌 맵으로 넘기기 (TxnId도 같이 넘김)
			GachaSys->SetPendingReveal(Pending.BannerID, Pending.TxnId, Pending.Results);
			UGameplayStatics::OpenLevel(this, FName(TEXT("GachaRevealMap")));
			return;
		}
	}
}

void ULRGachaShopWidget::NativeDestruct()
{
	// 델리게이트 해제
	if (GachaSys)
	{
		GachaSys->OnCurrencyChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	Super::NativeDestruct();
}

// ───────────────── 리빌 위젯 (구 플로우) ─────────────────

void ULRGachaShopWidget::ShowRevealWidget(FName InBannerID, const TArray<FLRGachaResult>& InResults)
{
	if (!RevealWidgetClass)
	{
		return;
	}

	UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (!UISys)
	{
		return;
	}

	ULRGachaRevealWidget* Reveal = UISys->OpenUI<ULRGachaRevealWidget>(RevealWidgetClass);
	if (!Reveal)
	{
		return;
	}

	// 변경 전 플로우: 바로 StartReveal (SkipState 없음, TxnId 불필요)
	Reveal->StartReveal(InBannerID, InResults);
}

// ───────────────── Draw 공통 처리 ─────────────────

void ULRGachaShopWidget::TryBeginDrawAndOpenReveal(FName BannerID, int32 Count)
{
	if (!GachaSys)
	{
		return;
	}

	// 새로고침: UI 표시와 실제 값이 어긋나지 않게 선반영
	RefreshPityText();

	int32 NeedCost = 0;
	if (!GachaSys->CanDraw(BannerID, Count, NeedCost))
	{
		// TODO: 토스트/팝업으로 “재화 부족” 표시
		UE_LOG(
			LogLRGachaShop,
			Warning,
			TEXT("[GachaShop] CanDraw Failed. Banner=%s Count=%d NeedCost=%d"),
			*BannerID.ToString(),
			Count,
			NeedCost
		);
		return;
	}

	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	// 트랜잭션 시작(비용 차감 + 결과 확정 저장)
	const bool bOk = GachaSys->BeginDrawTransaction(BannerID, Count, TxnId, Results);
	if (!bOk || !TxnId.IsValid() || Results.Num() == 0)
	{
		UE_LOG(
			LogLRGachaShop,
			Error,
			TEXT("[GachaShop] BeginDrawTransaction Failed. Banner=%s Count=%d"),
			*BannerID.ToString(),
			Count
		);
		return;
	}

	// 디버그 로그
	UE_LOG(
		LogLRGachaShop,
		Log,
		TEXT("[GachaShop] Txn Started. Banner=%s Count=%d Txn=%s Results=%d"),
		*BannerID.ToString(),
		Count,
		*TxnId.ToString(),
		Results.Num()
	);

	// 바로 커밋(지급 확정)
	// 연출 보는 도중 취소로 이득을 보는 걸 막기 위한 설계
	const bool bCommitted = GachaSys->CommitTransaction(TxnId);
	if (!bCommitted)
	{
		UE_LOG(
			LogLRGachaShop,
			Error,
			TEXT("[GachaShop] Auto-commit failed. Banner=%s Txn=%s"),
			*BannerID.ToString(),
			*TxnId.ToString()
		);
		return;
	}

	// 리빌 맵으로 결과 넘기기
	GachaSys->SetPendingReveal(BannerID, TxnId, Results);
	UGameplayStatics::OpenLevel(this, FName(TEXT("GachaRevealMap")));
}

// ───────────────── 탭 버튼 콜백 ─────────────────

void ULRGachaShopWidget::OnClickHeroTab()
{
	// Hero_FullMoon 기준
	CurrentBannerID = DefaultHeroBannerID;
	RefreshPityText();
}

void ULRGachaShopWidget::OnClickEquipTab()
{
	// Equip_FullMoon 기준
	CurrentBannerID = DefaultEquipBannerID;
	RefreshPityText();
}

// ───────────────── Draw 버튼 콜백 ─────────────────

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

// ───────────────── 델리게이트 핸들러 ─────────────────

void ULRGachaShopWidget::HandleCurrencyChanged(ELRCurrencyType Type, int32 /*NewValue*/)
{
	// 어떤 재화가 바뀌든 전체 텍스트를 갱신
	RefreshCurrencyTexts();
}

void ULRGachaShopWidget::HandlePityChanged(FName BannerID, int32 /*NewValue*/)
{
	// 표시 천장은 FullMoon만(탭에 따라 Hero/Equip)
	const FName DisplayBannerID = IsHeroTabSelected() ? DefaultHeroBannerID : DefaultEquipBannerID;

	if (BannerID == DisplayBannerID)
	{
		RefreshPityText();
	}
}

// ───────────────── UI 갱신 헬퍼 ─────────────────

void ULRGachaShopWidget::RefreshCurrencyTexts()
{
	if (!GachaSys)
	{
		return;
	}

	const int32 Gold = GachaSys->GetCurrency(ELRCurrencyType::Gold);
	const int32 Crescent = GachaSys->GetCurrency(ELRCurrencyType::CrescentTicket);
	const int32 FullMoon = GachaSys->GetCurrency(ELRCurrencyType::FullMoonTicket);

	if (TextGold)
	{
		TextGold->SetText(FText::AsNumber(Gold));
	}
	if (TextCrescent)
	{
		TextCrescent->SetText(FText::AsNumber(Crescent));
	}
	if (TextFullMoon)
	{
		TextFullMoon->SetText(FText::AsNumber(FullMoon));
	}
}

void ULRGachaShopWidget::RefreshPityText()
{
	if (!GachaSys || !TextPity)
	{
		return;
	}

	const ELRGachaItemType ItemType = IsHeroTabSelected()
		? ELRGachaItemType::Hero
		: ELRGachaItemType::Equipment;

	const int32 Pity = GachaSys->GetDisplayPityCount(ItemType);

	TextPity->SetText(
		FText::FromString(FString::Printf(TEXT("천장 카운트 : %d"), Pity))
	);
}

// ───────────────── 헬퍼 ─────────────────

bool ULRGachaShopWidget::IsHeroTabSelected() const
{
	// 배너 이름이 "Hero_" 로 시작하면 영웅 탭으로 판단 (현재 네이밍 규칙 기준)
	const FString BannerString = CurrentBannerID.ToString();
	return BannerString.StartsWith(TEXT("Hero_"));
}

FName ULRGachaShopWidget::MakeBannerIDForTicket(const bool bFullMoon) const
{
	// 배너 네이밍 규칙: Hero_FullMoon / Hero_Crescent / Equip_FullMoon / Equip_Crescent
	const bool bHero = IsHeroTabSelected();

	if (bHero)
	{
		return bFullMoon
			? FName(TEXT("Hero_FullMoon"))
			: FName(TEXT("Hero_Crescent"));
	}
	else
	{
		return bFullMoon
			? FName(TEXT("Equip_FullMoon"))
			: FName(TEXT("Equip_Crescent"));
	}
}