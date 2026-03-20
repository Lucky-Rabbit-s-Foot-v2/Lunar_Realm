// LRGachaShopWidget.cpp
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

	// ───────────────── 0) Subsystem 참조 ─────────────────
	if (UGameInstance* GI = GetGameInstance())
	{
		GachaSys = GI->GetSubsystem<ULRGachaSubsystem>();
	}

	// ───────────────── 1) 복구 목표값(Desired)만 계산 ─────────────────
	ELRGachaShopTab DesiredTab = ELRGachaShopTab::Hero;
	FName DesiredBannerID = DefaultHeroBannerID;

	if (GachaSys)
	{
		// 1순위: RequestOpenShopOnLobbyReturn로 저장된 마지막 배너
		const FName LastBanner = GachaSys->GetLastShopBanner();
		if (!LastBanner.IsNone())
		{
			DesiredBannerID = LastBanner;

			const FString BannerStr = LastBanner.ToString();
			DesiredTab = BannerStr.StartsWith(TEXT("Equip_"))
				? ELRGachaShopTab::Equip
				: ELRGachaShopTab::Hero;
		}
	}

	// 여기서는 아직 SetTab 호출하지 말고, 값만 저장
	CurrentTab = DesiredTab;
	CurrentBannerID = DesiredBannerID;

	// ───────────────── 3) 버튼 바인딩 ─────────────────
	if (ButtonHome)
	{
		ButtonHome->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickHome);
		ButtonHome->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickHome);
	}

	if (ButtonHeroTab)
	{
		ButtonHeroTab->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
		ButtonHeroTab->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
	}

	if (ButtonEquipTab)
	{
		ButtonEquipTab->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);
		ButtonEquipTab->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);
	}

	if (ButtonCrescentDraw1)
	{
		ButtonCrescentDraw1->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw1);
		ButtonCrescentDraw1->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw1);
	}
	if (ButtonCrescentDraw10)
	{
		ButtonCrescentDraw10->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw10);
		ButtonCrescentDraw10->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw10);
	}
	if (ButtonFullMoonDraw1)
	{
		ButtonFullMoonDraw1->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw1);
		ButtonFullMoonDraw1->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw1);
	}
	if (ButtonFullMoonDraw10)
	{
		ButtonFullMoonDraw10->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw10);
		ButtonFullMoonDraw10->OnClicked.AddUniqueDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw10);
	}

	// ───────────────── 4) 델리게이트 구독(재화/천장 변경) ─────────────────
	if (GachaSys)
	{
		GachaSys->OnCurrencyChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnCurrencyChanged.AddUniqueDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);

		GachaSys->OnPityChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
		GachaSys->OnPityChanged.AddUniqueDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	// ───────────────── 5) 최종 탭/배경/천장 갱신은 여기서 딱 1번 ─────────────────
	SetTab(CurrentTab);

	// ReturnBanner 같은 “세부 배너”까지 유지하려면 다시 덮어씀
	CurrentBannerID = DesiredBannerID;

	// 표시 천장 확정
	RefreshPityText();

	// ───────────────── 6) 튕김 복구(안전장치) ─────────────────
	if (GachaSys)
	{
		FLRGachaPendingTransaction Pending;
		if (GachaSys->GetAnyPendingTransaction(Pending))
		{
			UE_LOG(
				LogLRGachaShop,
				Warning,
				TEXT("[GachaShop] Pending Txn 발견 → 자동 커밋 후 리빌 재진입. Banner=%s Txn=%s Results=%d"),
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
					TEXT("[GachaShop] 자동 커밋 실패. Banner=%s Txn=%s"),
					*Pending.BannerID.ToString(),
					*Pending.TxnId.ToString()
				);
				return;
			}

			GachaSys->SetPendingReveal(Pending.BannerID, Pending.TxnId, Pending.Results);
			UGameplayStatics::OpenLevel(this, FName(TEXT("GachaRevealMap")));
			return;
		}
	}
}

void ULRGachaShopWidget::NativeDestruct()
{
	// ───────────────── 버튼 델리게이트 해제 ─────────────────
	if (ButtonHome)          ButtonHome->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickHome);
	if (ButtonHeroTab)       ButtonHeroTab->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickHeroTab);
	if (ButtonEquipTab)      ButtonEquipTab->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickEquipTab);
	if (ButtonCrescentDraw1) ButtonCrescentDraw1->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw1);
	if (ButtonCrescentDraw10)ButtonCrescentDraw10->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickCrescentDraw10);
	if (ButtonFullMoonDraw1) ButtonFullMoonDraw1->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw1);
	if (ButtonFullMoonDraw10)ButtonFullMoonDraw10->OnClicked.RemoveDynamic(this, &ULRGachaShopWidget::OnClickFullMoonDraw10);

	// ───────────────── Subsystem 델리게이트 해제 ─────────────────
	if (GachaSys)
	{
		GachaSys->OnCurrencyChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandleCurrencyChanged);
		GachaSys->OnPityChanged.RemoveDynamic(this, &ULRGachaShopWidget::HandlePityChanged);
	}

	Super::NativeDestruct();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Draw 공통 처리: 트랜잭션 시작 → 커밋 → PendingReveal → 레벨 이동
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::TryBeginDrawAndOpenReveal(FName BannerID, int32 Count)
{
	if (!GachaSys)
	{
		return;
	}

	// UI 표시와 실제 값 불일치 방지(선 갱신)
	RefreshPityText();

	int32 NeedCost = 0;
	if (!GachaSys->CanDraw(BannerID, Count, NeedCost))
	{
		// TODO: 토스트/팝업으로 “재화 부족” 표시
		UE_LOG(
			LogLRGachaShop,
			Warning,
			TEXT("[GachaShop] CanDraw 실패. Banner=%s Count=%d NeedCost=%d"),
			*BannerID.ToString(),
			Count,
			NeedCost
		);
		return;
	}

	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	// 1) 트랜잭션 시작: 비용 차감 + 결과 확정/저장 (지급은 아직 X)
	const bool bOk = GachaSys->BeginDrawTransaction(BannerID, Count, TxnId, Results);
	if (!bOk || !TxnId.IsValid() || Results.Num() == 0)
	{
		UE_LOG(
			LogLRGachaShop,
			Error,
			TEXT("[GachaShop] BeginDrawTransaction 실패. Banner=%s Count=%d"),
			*BannerID.ToString(),
			Count
		);
		return;
	}

	UE_LOG(
		LogLRGachaShop,
		Log,
		TEXT("[GachaShop] Txn 시작. Banner=%s Count=%d Txn=%s Results=%d"),
		*BannerID.ToString(),
		Count,
		*TxnId.ToString(),
		Results.Num()
	);

	// 2) 즉시 커밋(지급 확정)
	// - 연출 도중 앱 종료/취소로 이득을 보는 것을 막기 위한 정책
	const bool bCommitted = GachaSys->CommitTransaction(TxnId);
	if (!bCommitted)
	{
		UE_LOG(
			LogLRGachaShop,
			Error,
			TEXT("[GachaShop] Auto-commit 실패. Banner=%s Txn=%s"),
			*BannerID.ToString(),
			*TxnId.ToString()
		);
		return;
	}

	// 3) 리빌 맵으로 넘길 결과를 Subsystem에 캐시
	GachaSys->SetPendingReveal(BannerID, TxnId, Results);

	// 4) 레벨 전환 전에 Shop UI 닫기
	// - UIManager 캐시에 “열림 상태”가 남아 검은 화면/입력 꼬임이 생기는 것을 방지
	if (UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UISys->CloseUI(this);
	}
	else
	{
		RemoveFromParent();
	}

	// 5) 리빌 맵으로 이동
	UGameplayStatics::OpenLevel(this, FName(TEXT("GachaRevealMap")));
}

// ─────────────────────────────────────────────────────────────────────────────
//  탭 버튼
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::OnClickHeroTab()
{
	PlayUISound(TabClickSound);
	SetTab(ELRGachaShopTab::Hero);
}

void ULRGachaShopWidget::OnClickEquipTab()
{
	PlayUISound(TabClickSound);
	SetTab(ELRGachaShopTab::Equip);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Draw 버튼
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::OnClickCrescentDraw1()
{
	PlayUISound(DrawClickSound);
	const FName BannerID = MakeBannerIDForTicket(false);
	TryBeginDrawAndOpenReveal(BannerID, 1);
}

void ULRGachaShopWidget::OnClickCrescentDraw10()
{
	PlayUISound(DrawClickSound);
	const FName BannerID = MakeBannerIDForTicket(false);
	TryBeginDrawAndOpenReveal(BannerID, 10);
}

void ULRGachaShopWidget::OnClickFullMoonDraw1()
{
	PlayUISound(DrawClickSound);
	const FName BannerID = MakeBannerIDForTicket(true);
	TryBeginDrawAndOpenReveal(BannerID, 1);
}

void ULRGachaShopWidget::OnClickFullMoonDraw10()
{
	PlayUISound(DrawClickSound);
	const FName BannerID = MakeBannerIDForTicket(true);
	TryBeginDrawAndOpenReveal(BannerID, 10);
}

// ─────────────────────────────────────────────────────────────────────────────
//  델리게이트 핸들러(서브시스템 이벤트)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::HandleCurrencyChanged(ELRCurrencyType Type, int32 /*NewValue*/)
{
	// 어떤 재화가 변하든 상단 표시 전체를 갱신
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

// ─────────────────────────────────────────────────────────────────────────────
//  UI 갱신
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::RefreshCurrencyTexts()
{
	if (!GachaSys)
	{
		return;
	}

	const int32 Gold = GachaSys->GetCurrency(ELRCurrencyType::Gold);
	const int32 Crescent = GachaSys->GetCurrency(ELRCurrencyType::CrescentTicket);
	const int32 FullMoon = GachaSys->GetCurrency(ELRCurrencyType::FullMoonTicket);

	if (TextGold)     TextGold->SetText(FText::AsNumber(Gold));
	if (TextCrescent) TextCrescent->SetText(FText::AsNumber(Crescent));
	if (TextFullMoon) TextFullMoon->SetText(FText::AsNumber(FullMoon));
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

	const int32 Current = GachaSys->GetDisplayPityCount(ItemType);
	const int32 Max = 100;

	TextPity->SetText(
		FText::FromString(FString::Printf(TEXT("%d/%d"), Current, Max))
	);
}

// ─────────────────────────────────────────────────────────────────────────────
//  배너 계산 헬퍼
// ─────────────────────────────────────────────────────────────────────────────


bool ULRGachaShopWidget::IsHeroTabSelected() const
{
	return CurrentTab == ELRGachaShopTab::Hero;
}


FName ULRGachaShopWidget::MakeBannerIDForTicket(const bool bFullMoon) const
{
	// 배너 네이밍 규칙:
	// Hero_FullMoon / Hero_Crescent / Equip_FullMoon / Equip_Crescent
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

// ─────────────────────────────────────────────────────────────────────────────
//  Home 버튼(로비로 돌아가기)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaShopWidget::OnClickHome()
{
	PlayUISound(HomeClickSound);

	// 로비 복귀 시 “샵 자동 오픈” 플래그 끄기
	// - 사용자가 홈으로 빠지는 경우는 샵 자동 재오픈이 UX에 방해될 수 있음
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULRGachaSubsystem* LocalGachaSys = GI->GetSubsystem<ULRGachaSubsystem>())
		{
			LocalGachaSys->ClearOpenShopOnLobbyReturn();
		}
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// PIE prefix 제거해서 맵 이름 정규화
	FString MapName = World->GetMapName();
	const FString Prefix = World->StreamingLevelsPrefix;
	if (!Prefix.IsEmpty())
	{
		MapName.RemoveFromStart(Prefix);
	}

	// 1) UI 닫기(먼저 닫아야 검은 화면/입력 꼬임이 덜함)
	if (UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UISys->CloseUI(this);
	}
	else
	{
		RemoveFromParent();
	}

	// 2) 이미 로비 맵이면 레벨 이동 X, 로비 페이지로 전환만
	if (MapName.Equals(TEXT("Map_Lobby"), ESearchCase::IgnoreCase))
	{
		if (UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
		{
			UISys->OpenUIByID(EUIID::LOBBY);
		}
		return;
	}

	// 3) 로비 맵이 아니면 로비로 이동
	UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
}

void ULRGachaShopWidget::SetTab(ELRGachaShopTab NewTab)
{
	CurrentTab = NewTab;

	// 탭에 따라 “표시용 배너 기준”도 동기화
	CurrentBannerID = (CurrentTab == ELRGachaShopTab::Hero)
		? DefaultHeroBannerID
		: DefaultEquipBannerID;

	// UI 값 갱신(천장/재화 등)
	RefreshCurrencyTexts();
	RefreshPityText();

	// BP에게 “배경만” 적용하라고 신호
	BP_ApplyTabBackground(CurrentTab);

	// 탭 변경이 일어났으면 Subsystem에 마지막 선택 배너 저장
	if (GachaSys)
	{
		GachaSys->SetLastShopBanner(CurrentBannerID);
	}
}

void ULRGachaShopWidget::PlayUISound(USoundBase* InSound)
{
	if (InSound)
	{
		UGameplayStatics::PlaySound2D(this, InSound);
	}
}