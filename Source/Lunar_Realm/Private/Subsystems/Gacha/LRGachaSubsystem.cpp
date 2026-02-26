// LRGachaSubsystem.cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

#include "SaveGame/LRSaveGame.h"
#include "System/LoggingSystem.h"

#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Subsystem 생명주기
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 맵 로드 완료 후 훅(로비 복귀 시 샵 자동 오픈 처리)
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULRGachaSubsystem::HandlePostLoadMapWithWorld);

	// ── DataTable 기본 경로(에디터에서 지정 안 했을 때) ───────────
	if (BannerDataTable.IsNull())
	{
		BannerDataTable = TSoftObjectPtr<UDataTable>(
			FSoftObjectPath(TEXT("DataTable'/Game/DataTables/Gacha/DT_GachaBanners.DT_GachaBanners'"))
		);
	}
	if (PoolDataTable.IsNull())
	{
		PoolDataTable = TSoftObjectPtr<UDataTable>(
			FSoftObjectPath(TEXT("DataTable'/Game/DataTables/Gacha/DT_GachaPool.DT_GachaPool'"))
		);
	}
	if (DuplicateRewardDataTable.IsNull())
	{
		DuplicateRewardDataTable = TSoftObjectPtr<UDataTable>(
			FSoftObjectPath(TEXT("DataTable'/Game/DataTables/Gacha/DT_GachaDuplicateRewards.DT_GachaDuplicateRewards'"))
		);
	}
	if (RarityRateDataTable.IsNull())
	{
		RarityRateDataTable = TSoftObjectPtr<UDataTable>(
			FSoftObjectPath(TEXT("DataTable'/Game/DataTables/Gacha/DT_GachaRarityRates.DT_GachaRarityRates'"))
		);
	}

	// ── Subsystem 참조 캐시 ───────────────────────────────────────
	if (UGameInstance* GI = GetGameInstance())
	{
		CurrencySubsystem = GI->GetSubsystem<UCurrencySubsystem>();
		SaveGameSubsystem = GI->GetSubsystem<USaveGameSubsystem>();
	}

	// ── DataTable 로드 ───────────────────────────────────────────
	LoadDataTables();
	LR_INFO(TEXT("LRGachaSubsystem Initialized"));

	// ── SaveGame에 Pending 트랜잭션이 남아있으면 상태를 PendingReveal로 ──
	// (튕김 복구 흐름: Shop 진입 시 GetAnyPendingTransaction로 처리)
	if (ULRSaveGame* S = GetSaveGame())
	{
		if (S->GachaPendingTransactions.Num() > 0)
		{
			bTxnInProgress = true;
			OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);
		}
	}
}

void ULRGachaSubsystem::Deinitialize()
{
	// 델리게이트 등록 해제(PIE 재실행/중복 등록 방지)
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────────────────────────
//  로비 복귀 시 샵 자동 오픈 플로우
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaSubsystem::RequestOpenShopOnLobbyReturn(FName InBannerID)
{
	bOpenShopOnLobbyReturn = true;
	PendingReturnShopBannerID = InBannerID;
}

void ULRGachaSubsystem::ClearOpenShopOnLobbyReturn()
{
	// Home 버튼 등으로 “샵 자동 오픈”이 필요 없는 경우 호출
	bOpenShopOnLobbyReturn = false;
	PendingReturnShopBannerID = NAME_None;
}

bool ULRGachaSubsystem::ConsumePendingReturnShopBanner(FName& OutBannerID)
{
	// “복귀 배너”는 한 번만 소비되도록 설계
	if (PendingReturnShopBannerID.IsNone())
	{
		return false;
	}

	OutBannerID = PendingReturnShopBannerID;
	PendingReturnShopBannerID = NAME_None;
	return true;
}

void ULRGachaSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!LoadedWorld || !bOpenShopOnLobbyReturn)
	{
		return;
	}

	// PIE에서는 "UEDPIE_0_Map_Lobby" 같은 이름이 될 수 있으니 prefix 제거
	FString MapName = LoadedWorld->GetMapName();
	const FString Prefix = LoadedWorld->StreamingLevelsPrefix;
	if (!Prefix.IsEmpty())
	{
		MapName.RemoveFromStart(Prefix);
	}

	// 로비 맵이 아닐 때는 무시
	if (!MapName.Equals(TEXT("Map_Lobby")))
	{
		return;
	}

	// 플래그는 1회성으로 꺼준다
	bOpenShopOnLobbyReturn = false;

	UGameInstance* GI = LoadedWorld->GetGameInstance();
	if (!GI)
	{
		return;
	}

	UUIManagerSubsystem* UISys = GI->GetSubsystem<UUIManagerSubsystem>();
	if (!UISys)
	{
		return;
	}

	// UIManagerSettings에서 EUIID::GACHA가 WBP_GachaShop으로 매핑되어 있어야 함
	UISys->OpenUIByID(EUIID::GACHA);
}

// ─────────────────────────────────────────────────────────────────────────────
//  DataTable 로드/SaveGame 헬퍼
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaSubsystem::LoadDataTables()
{
	LoadedBannerDT = BannerDataTable.IsNull() ? nullptr : Cast<UDataTable>(BannerDataTable.LoadSynchronous());
	LoadedPoolDT = PoolDataTable.IsNull() ? nullptr : Cast<UDataTable>(PoolDataTable.LoadSynchronous());
	LoadedDupRewardDT = DuplicateRewardDataTable.IsNull() ? nullptr : Cast<UDataTable>(DuplicateRewardDataTable.LoadSynchronous());
	LoadedRarityRateDT = RarityRateDataTable.IsNull() ? nullptr : Cast<UDataTable>(RarityRateDataTable.LoadSynchronous());

	LR_INFO(
		TEXT("Gacha DT Loaded: Banner=%s Pool=%s Dup=%s Rate=%s"),
		LoadedBannerDT ? TEXT("OK") : TEXT("NULL"),
		LoadedPoolDT ? TEXT("OK") : TEXT("NULL"),
		LoadedDupRewardDT ? TEXT("OK") : TEXT("NULL"),
		LoadedRarityRateDT ? TEXT("OK") : TEXT("NULL")
	);
}

ULRSaveGame* ULRGachaSubsystem::GetSaveGame() const
{
	return SaveGameSubsystem ? SaveGameSubsystem->GetCurrentSaveGame() : nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  재화 API
// ─────────────────────────────────────────────────────────────────────────────

int32 ULRGachaSubsystem::GetCurrency(ELRCurrencyType Type) const
{
	return CurrencySubsystem ? CurrencySubsystem->GetCurrency(Type) : 0;
}

void ULRGachaSubsystem::AddCurrency(ELRCurrencyType Type, int32 Delta)
{
	if (!CurrencySubsystem)
	{
		return;
	}

	CurrencySubsystem->AddCurrency(Type, Delta);
	OnCurrencyChanged.Broadcast(Type, GetCurrency(Type));
}

bool ULRGachaSubsystem::SpendCurrency(ELRCurrencyType Type, int32 Cost)
{
	if (!CurrencySubsystem)
	{
		return false;
	}

	const bool bOk = CurrencySubsystem->SpendCurrency(Type, Cost);
	if (bOk)
	{
		OnCurrencyChanged.Broadcast(Type, GetCurrency(Type));
	}
	return bOk;
}

// ─────────────────────────────────────────────────────────────────────────────
//  천장(Pity)
// ─────────────────────────────────────────────────────────────────────────────

int32 ULRGachaSubsystem::GetPityCount(FName BannerID) const
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return 0;
	}

	if (const int32* Found = S->GachaPityCounterMap.Find(BannerID))
	{
		return *Found;
	}
	return 0;
}

void ULRGachaSubsystem::IncrementPity(FName BannerID)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return;
	}

	int32& PityValue = S->GachaPityCounterMap.FindOrAdd(BannerID);
	PityValue++;

	OnPityChanged.Broadcast(BannerID, PityValue);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}
}

void ULRGachaSubsystem::ResetPity(FName BannerID)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return;
	}

	int32& PityValue = S->GachaPityCounterMap.FindOrAdd(BannerID);
	PityValue = 0;

	OnPityChanged.Broadcast(BannerID, PityValue);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  DataTable 조회
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::GetBannerRow(FName BannerID, FLRGachaBannerRow& OutRow) const
{
	if (!LoadedBannerDT)
	{
		return false;
	}

	if (FLRGachaBannerRow* Row = LoadedBannerDT->FindRow<FLRGachaBannerRow>(BannerID, TEXT("GachaBannerLookup")))
	{
		OutRow = *Row;
		return true;
	}
	return false;
}

void ULRGachaSubsystem::GetPoolRowsForBanner(FName BannerID, TArray<FLRGachaPoolRow>& OutRows) const
{
	OutRows.Empty();
	if (!LoadedPoolDT)
	{
		return;
	}

	const TArray<FName> RowNames = LoadedPoolDT->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		if (FLRGachaPoolRow* Row = LoadedPoolDT->FindRow<FLRGachaPoolRow>(RowName, TEXT("GachaPoolLookup")))
		{
			if (Row->BannerID == BannerID)
			{
				OutRows.Add(*Row);
			}
		}
	}
}

void ULRGachaSubsystem::GetRarityRateRowsForBanner(
	FName BannerID,
	ELRGachaItemType ItemType,
	TArray<FLRGachaRarityRateRow>& OutRows) const
{
	OutRows.Empty();
	if (!LoadedRarityRateDT)
	{
		return;
	}

	const TArray<FName> RowNames = LoadedRarityRateDT->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FLRGachaRarityRateRow* Row = LoadedRarityRateDT->FindRow<FLRGachaRarityRateRow>(RowName, TEXT("RarityRateLookup"));
		if (!Row)
		{
			continue;
		}

		if (Row->BannerID == BannerID && Row->ItemType == ItemType)
		{
			OutRows.Add(*Row);
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  희귀도/풀 선택 로직
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const
{
	OutRarity = ELRGachaRarity::Common;

	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, ItemType, Rates);

	if (Rates.Num() <= 0)
	{
		return false;
	}

	float Total = 0.0f;
	for (const auto& R : Rates)
	{
		Total += FMath::Max(0.0f, R.Rate);
	}
	if (Total <= 0.0f)
	{
		return false;
	}

	const float Rand = FMath::FRandRange(0.0f, Total);
	float Acc = 0.0f;

	for (const auto& R : Rates)
	{
		Acc += FMath::Max(0.0f, R.Rate);
		if (Rand <= Acc)
		{
			OutRarity = R.Rarity;
			return true;
		}
	}

	return false;
}

bool ULRGachaSubsystem::PickOneFromPoolUniform(const TArray<FLRGachaPoolRow>& Pool, FLRGachaPoolRow& OutPicked) const
{
	if (Pool.Num() <= 0)
	{
		return false;
	}

	const int32 Index = FMath::RandRange(0, Pool.Num() - 1);
	OutPicked = Pool[Index];
	return true;
}

bool ULRGachaSubsystem::PickOneFromPoolByRarity(
	const TArray<FLRGachaPoolRow>& Pool,
	ELRGachaRarity TargetRarity,
	FLRGachaPoolRow& OutPicked) const
{
	TArray<FLRGachaPoolRow> Filtered;
	for (const auto& Row : Pool)
	{
		if (Row.Rarity == TargetRarity)
		{
			Filtered.Add(Row);
		}
	}

	// 해당 등급이 없으면 전체 풀에서라도 뽑도록 fallback
	return PickOneFromPoolUniform((Filtered.Num() > 0) ? Filtered : Pool, OutPicked);
}

// ─────────────────────────────────────────────────────────────────────────────
//  중복 보상(골드 전환)
// ─────────────────────────────────────────────────────────────────────────────

int32 ULRGachaSubsystem::GetDuplicateGold(ELRGachaRarity Rarity) const
{
	if (!LoadedDupRewardDT)
	{
		return 0;
	}

	for (const FName& RowName : LoadedDupRewardDT->GetRowNames())
	{
		if (const FLRGachaDuplicateRewardRow* Row =
			LoadedDupRewardDT->FindRow<FLRGachaDuplicateRewardRow>(RowName, TEXT("DupRewardLookup")))
		{
			if (Row->Rarity == Rarity)
			{
				return Row->GoldAmount;
			}
		}
	}
	return 0;
}

bool ULRGachaSubsystem::TryAddToCollection(const FLRGachaResult& Result, bool& bOutWasNew)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	UCollectionSubsystem* CollectionSys = GI->GetSubsystem<UCollectionSubsystem>();
	if (!CollectionSys)
	{
		return false;
	}

	bOutWasNew = false;

	if (Result.ItemType == ELRGachaItemType::Hero)
	{
		if (!CollectionSys->HasCharacter(Result.ItemID))
		{
			CollectionSys->AddCharacter(Result.ItemID, 1);
			bOutWasNew = true;
		}
	}
	else
	{
		if (!CollectionSys->HasEquipment(Result.ItemID))
		{
			CollectionSys->AddEquipment(Result.ItemID, 1);
			bOutWasNew = true;
		}
	}

	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  뽑기 가능 여부
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::CanDraw(FName BannerID, int32 DrawCount, int32& OutNeedCost) const
{
	OutNeedCost = 0;

	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		return false;
	}

	// 1회/10회 외 값은 1회 비용 * Count로 처리
	if (DrawCount == 1)
	{
		OutNeedCost = Banner.CostSingle;
	}
	else if (DrawCount == 10)
	{
		OutNeedCost = Banner.CostTen;
	}
	else
	{
		OutNeedCost = Banner.CostSingle * DrawCount;
	}

	return GetCurrency(Banner.CostCurrencyType) >= OutNeedCost;
}

// ─────────────────────────────────────────────────────────────────────────────
//  트랜잭션: Begin(비용 차감 + 결과 확정 저장)
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults)
{
	OutTxnId.Invalidate();
	OutResults.Empty();

	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: SaveGame null"));
		return false;
	}

	// 연타/중복 실행 방지(락)
	if (bTxnInProgress)
	{
		LR_WARN(TEXT("BeginDrawTransaction 차단: 이미 트랜잭션 진행 중"));
		return false;
	}

	// Pending이 남아있으면 새 트랜잭션 시작 금지
	if (S->GachaPendingTransactions.Num() > 0)
	{
		LR_WARN(TEXT("BeginDrawTransaction 차단: Pending 트랜잭션이 남아있음"));
		return false;
	}

	// 배너 로드
	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: 배너 없음 %s"), *BannerID.ToString());
		return false;
	}

	// 비용 체크
	int32 NeedCost = 0;
	if (!CanDraw(BannerID, DrawCount, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: 재화 부족"));
		return false;
	}

	// 비용 차감(예약)
	if (!SpendCurrency(Banner.CostCurrencyType, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: SpendCurrency 실패"));
		return false;
	}

	// 뽑기 풀 로드
	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		// 차감했는데 풀 없음 → 환불
		AddCurrency(Banner.CostCurrencyType, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction 실패: Pool 비어있음 → 환불 처리"));
		return false;
	}

	// 확률 테이블 유효성 검증
	{
		TArray<FLRGachaRarityRateRow> Rates;
		GetRarityRateRowsForBanner(BannerID, Banner.ItemType, Rates);

		float Total = 0.f;
		for (const auto& R : Rates)
		{
			Total += FMath::Max(0.f, R.Rate);
		}

		if (Rates.Num() <= 0 || Total <= 0.f)
		{
			AddCurrency(Banner.CostCurrencyType, NeedCost);
			LR_WARN(TEXT("BeginDrawTransaction 실패: 확률 테이블 누락/무효 → 환불 처리"));
			return false;
		}
	}

	// 천장 계산용 작업용 pity(실제 저장값은 커밋 전용으로 유지)
	const int32 PrevPity = GetPityCount(BannerID);
	int32 WorkingPity = PrevPity;

	// 결과 롤링(지급X)
	if (!RollResults_NoApply(Banner, Pool, DrawCount, WorkingPity, OutResults))
	{
		AddCurrency(Banner.CostCurrencyType, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction 실패: 롤링 실패 → 환불 처리"));
		return false;
	}

	// 천장 카운터는 “결과 확정 시점”에 저장
	if (Banner.bUsePity)
	{
		S->GachaPityCounterMap.FindOrAdd(BannerID) = WorkingPity;
		OnPityChanged.Broadcast(BannerID, WorkingPity);
	}

	// Pending 트랜잭션 저장(튕김 복구 핵심)
	FLRGachaPendingTransaction Pending;
	Pending.TxnId = FGuid::NewGuid();
	Pending.BannerID = BannerID;
	Pending.DrawCount = DrawCount;
	Pending.CostCurrencyType = Banner.CostCurrencyType;
	Pending.CostAmount = NeedCost;
	Pending.PrevPity = PrevPity;
	Pending.NewPity = WorkingPity;
	Pending.Results = OutResults;
	Pending.State = ELRGachaTxnState::PendingReveal;

	S->GachaPendingTransactions.Add(Pending.TxnId, Pending);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}

	// 락 ON + 상태 변경 브로드캐스트
	bTxnInProgress = true;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);

	// UI 연출용 이벤트(원하면 여기 구독해서 바로 리빌 열 수 있음)
	OnGachaFinished.Broadcast(BannerID, OutResults);

	OutTxnId = Pending.TxnId;

	// 디버그 출력
	DebugPrintResults(BannerID, DrawCount, OutTxnId, OutResults);

	LR_INFO(
		TEXT("BeginDrawTransaction OK: %s Txn=%s Cost=%d PrevPity=%d NewPity=%d"),
		*BannerID.ToString(),
		*OutTxnId.ToString(),
		NeedCost,
		PrevPity,
		WorkingPity
	);

	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  트랜잭션: Commit(실제 지급)
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::CommitTransaction(const FGuid& TxnId)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return false;
	}

	FLRGachaPendingTransaction* PendingPtr = S->GachaPendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CommitTransaction 실패: Pending 없음"));
		return false;
	}

	// Remove 전에 필요한 값 복사(안전)
	const FName BannerIDCopy = PendingPtr->BannerID;
	TArray<FLRGachaResult> ResultsCopy = PendingPtr->Results;

	// 실제 지급(컬렉션 반영 + 중복이면 골드 지급)
	for (const FLRGachaResult& Result : ResultsCopy)
	{
		bool bWasNew = false;
		if (!TryAddToCollection(Result, bWasNew))
		{
			LR_WARN(TEXT("CommitTransaction: TryAddToCollection 실패"));
			continue;
		}

		if (!bWasNew)
		{
			const int32 Gold = GetDuplicateGold(Result.Rarity);
			AddCurrency(ELRCurrencyType::Gold, Gold);
		}
	}

	// Pending 제거 + 저장
	S->GachaPendingTransactions.Remove(TxnId);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}

	// 락 해제 + 상태 브로드캐스트
	bTxnInProgress = false;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::Committed);

	LR_INFO(TEXT("CommitTransaction OK: %s Txn=%s"), *BannerIDCopy.ToString(), *TxnId.ToString());
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  트랜잭션: Cancel(환불 + 천장 원복)
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::CancelTransaction(const FGuid& TxnId)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return false;
	}

	FLRGachaPendingTransaction* PendingPtr = S->GachaPendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CancelTransaction 실패: Pending 없음"));
		return false;
	}

	const FName BannerIDCopy = PendingPtr->BannerID;
	const ELRCurrencyType CostTypeCopy = PendingPtr->CostCurrencyType;
	const int32 CostAmountCopy = PendingPtr->CostAmount;
	const int32 PrevPityCopy = PendingPtr->PrevPity;

	// 환불
	AddCurrency(CostTypeCopy, CostAmountCopy);

	// 천장 원복
	S->GachaPityCounterMap.FindOrAdd(BannerIDCopy) = PrevPityCopy;
	OnPityChanged.Broadcast(BannerIDCopy, PrevPityCopy);

	// Pending 제거 + 저장
	S->GachaPendingTransactions.Remove(TxnId);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}

	// 락 해제 + 상태 브로드캐스트
	bTxnInProgress = false;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::Canceled);

	LR_INFO(TEXT("CancelTransaction OK: %s Txn=%s Refunded=%d"), *BannerIDCopy.ToString(), *TxnId.ToString(), CostAmountCopy);
	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  결과 롤링(지급X) - 신규/중복 판정 포함
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::RollResults_NoApply(
	const FLRGachaBannerRow& Banner,
	const TArray<FLRGachaPoolRow>& Pool,
	int32 DrawCount,
	int32& InOutPityCounter,
	TArray<FLRGachaResult>& OutResults
)
{
	OutResults.Empty();

	UCollectionSubsystem* CollectionSys = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	if (!CollectionSys)
	{
		LR_WARN(TEXT("RollResults_NoApply 실패: CollectionSubsystem 없음"));
		return false;
	}

	// “이번 트랜잭션 안에서” 같은 아이템이 중복으로 나온 경우도 중복 처리하기 위한 세트
	TSet<FName> NewlyObtainedInThisTxn;

	for (int32 i = 0; i < DrawCount; i++)
	{
		const bool bShouldPity = (Banner.bUsePity && (InOutPityCounter + 1) >= Banner.PityThreshold);

		FLRGachaPoolRow Picked;
		bool bPickedOk = false;

		if (bShouldPity)
		{
			// 천장 발동: 확정 등급에서 뽑기
			bPickedOk = PickOneFromPoolByRarity(Pool, Banner.PityGuaranteedRarity, Picked);
		}
		else
		{
			// 일반: 확률로 등급 뽑고, 해당 등급 풀에서 랜덤
			ELRGachaRarity RolledRarity = ELRGachaRarity::Common;
			const bool bHasRate = PickRarityByRates(Banner.BannerID, Banner.ItemType, RolledRarity);

			if (!bHasRate)
			{
				LR_WARN(TEXT("RollResults_NoApply 실패: PickRarityByRates 실패 (banner=%s)"), *Banner.BannerID.ToString());
				return false;
			}

			TArray<FLRGachaPoolRow> Filtered;
			for (const auto& Row : Pool)
			{
				if (Row.Rarity == RolledRarity)
				{
					Filtered.Add(Row);
				}
			}

			bPickedOk = PickOneFromPoolUniform((Filtered.Num() > 0) ? Filtered : Pool, Picked);
		}

		if (!bPickedOk)
		{
			LR_WARN(TEXT("RollResults_NoApply 실패: pick 실패"));
			return false;
		}

		FLRGachaResult Result;
		Result.ItemType = Banner.ItemType;
		Result.ItemID = Picked.ItemID;
		Result.Rarity = Picked.Rarity;

		// “지급 전” 기준 신규/중복 판정
		bool bOwnedAlready = false;
		if (Result.ItemType == ELRGachaItemType::Hero)
		{
			bOwnedAlready = CollectionSys->HasCharacter(Result.ItemID);
		}
		else
		{
			bOwnedAlready = CollectionSys->HasEquipment(Result.ItemID);
		}

		const bool bDupInThisTxn = NewlyObtainedInThisTxn.Contains(Result.ItemID);

		if (!bOwnedAlready && !bDupInThisTxn)
		{
			Result.bIsNew = true;
			NewlyObtainedInThisTxn.Add(Result.ItemID);
		}
		else
		{
			// 중복이면 골드 전환 정보도 미리 기록(결과 UI에서 사용 가능)
			Result.bIsNew = false;
			Result.bConvertedToGold = true;
			Result.ConvertedGoldAmount = GetDuplicateGold(Result.Rarity);
		}

		// 천장 카운터 갱신(전설이면 리셋)
		if (Banner.bUsePity)
		{
			if (Result.Rarity == ELRGachaRarity::Legendary)
			{
				InOutPityCounter = 0;
			}
			else
			{
				InOutPityCounter++;
			}
		}

		OutResults.Add(Result);
	}

	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Pending 트랜잭션 조회(튕김 복구용)
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const
{
	OutPending = FLRGachaPendingTransaction();

	ULRSaveGame* S = GetSaveGame();
	if (!S || S->GachaPendingTransactions.Num() <= 0)
	{
		return false;
	}

	// 하나만 쓰면 되므로 첫 번째 반환
	for (const auto& Pair : S->GachaPendingTransactions)
	{
		OutPending = Pair.Value;
		return true;
	}
	return false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  UI 편의 함수
// ─────────────────────────────────────────────────────────────────────────────

FName ULRGachaSubsystem::GetBannerIdBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType) const
{
	const bool bFullMoon = (TicketType == ELRGachaTicketType::FullMoon);

	if (ItemType == ELRGachaItemType::Hero)
	{
		return bFullMoon ? FName(TEXT("Hero_FullMoon")) : FName(TEXT("Hero_Crescent"));
	}
	else
	{
		return bFullMoon ? FName(TEXT("Equip_FullMoon")) : FName(TEXT("Equip_Crescent"));
	}
}

bool ULRGachaSubsystem::BeginDrawBySelection(
	ELRGachaItemType ItemType,
	ELRGachaTicketType TicketType,
	int32 DrawCount,
	FGuid& OutTxnId,
	TArray<FLRGachaResult>& OutResults
)
{
	const FName BannerID = GetBannerIdBySelection(ItemType, TicketType);

	const bool bOk = BeginDrawTransaction(BannerID, DrawCount, OutTxnId, OutResults);
	if (!bOk)
	{
		LR_WARN(TEXT("[Gacha] BeginDrawBySelection 실패 Banner=%s Count=%d"), *BannerID.ToString(), DrawCount);
	}
	return bOk;
}

int32 ULRGachaSubsystem::GetDisplayPityCount(ELRGachaItemType ItemType) const
{
	const FName FullMoonBannerID =
		(ItemType == ELRGachaItemType::Hero)
		? FName(TEXT("Hero_FullMoon"))
		: FName(TEXT("Equip_FullMoon"));

	return GetPityCount(FullMoonBannerID);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Pending Reveal (리빌 맵 연동)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaSubsystem::SetPendingReveal(FName InBannerID, const FGuid& InTxnId, const TArray<FLRGachaResult>& InResults)
{
	PendingBannerID = InBannerID;
	PendingTxnId = InTxnId;
	PendingResults = InResults;
	bHasPendingReveal = (PendingResults.Num() > 0);
}

bool ULRGachaSubsystem::ConsumePendingReveal(FName& OutBannerID, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults)
{
	if (!bHasPendingReveal || PendingResults.Num() == 0)
	{
		return false;
	}

	OutBannerID = PendingBannerID;
	OutTxnId = PendingTxnId;
	OutResults = MoveTemp(PendingResults);

	// 소비 후 상태 리셋(한 번만 사용)
	PendingResults.Reset();
	PendingBannerID = NAME_None;
	PendingTxnId.Invalidate();
	bHasPendingReveal = false;

	return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Debug 출력(로그/화면)
// ─────────────────────────────────────────────────────────────────────────────

FLinearColor ULRGachaSubsystem::DebugRarityToColor(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
	case ELRGachaRarity::Elite:     return FLinearColor(0.20f, 0.55f, 1.00f, 1.0f);
	case ELRGachaRarity::Unique:    return FLinearColor(1.00f, 0.25f, 0.25f, 1.0f);
	case ELRGachaRarity::Epic:      return FLinearColor(0.6f, 0.2f, 0.8f, 1.0f);
	case ELRGachaRarity::Legendary: return FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
	default:                        return FLinearColor::White;
	}
}

FString ULRGachaSubsystem::DebugResultToColoredString(const FLRGachaResult& Result) const
{
	const UEnum* ItemTypeEnum = StaticEnum<ELRGachaItemType>();
	const UEnum* RarityEnum = StaticEnum<ELRGachaRarity>();

	const FString ItemTypeStr = ItemTypeEnum ? ItemTypeEnum->GetNameStringByValue((int64)Result.ItemType) : TEXT("UnknownType");
	const FString RarityStr = RarityEnum ? RarityEnum->GetNameStringByValue((int64)Result.Rarity) : TEXT("UnknownRarity");

	FString ColorName = TEXT("White");
	switch (Result.Rarity)
	{
	case ELRGachaRarity::Common:    ColorName = TEXT("Gray");   break;
	case ELRGachaRarity::Elite:     ColorName = TEXT("Blue");   break;
	case ELRGachaRarity::Unique:    ColorName = TEXT("Red");    break;
	case ELRGachaRarity::Epic:      ColorName = TEXT("Purple"); break;
	case ELRGachaRarity::Legendary: ColorName = TEXT("Gold");   break;
	}

	return FString::Printf(
		TEXT("%s | %s(%s) | ItemID=%s | New=%d | GoldConv=%d(+%d)"),
		*ItemTypeStr,
		*RarityStr, *ColorName,
		*Result.ItemID.ToString(),
		Result.bIsNew ? 1 : 0,
		Result.bConvertedToGold ? 1 : 0,
		Result.ConvertedGoldAmount
	);
}

void ULRGachaSubsystem::DebugPrintResults(FName BannerID, int32 DrawCount, const FGuid& TxnId, const TArray<FLRGachaResult>& Results) const
{
	// ── 1) 로그 출력(파일/콘솔) ───────────────────────────────────
	if (bDebugPrintToLog)
	{
		LR_INFO(TEXT("[GachaDebug] Banner=%s Count=%d Txn=%s Results=%d"),
			*BannerID.ToString(),
			DrawCount,
			*TxnId.ToString(),
			Results.Num()
		);

		for (int32 i = 0; i < Results.Num(); ++i)
		{
			LR_INFO(TEXT("[GachaDebug] #%02d %s"), i + 1, *DebugResultToColoredString(Results[i]));
		}
	}

	// ── 2) 화면 출력(좌상단 OnScreenDebugMessage) ─────────────────
	if (bDebugPrintToScreen && GEngine)
	{
		// 누적 출력 방지
		// 기존 메시지를 지우고,
		// 고정된 Key로 덮어쓰기
		GEngine->ClearOnScreenDebugMessages();

		const int32 BaseKey = 9000; // 고정 키 범위(프로젝트에서 다른 디버그 키와 겹치지만 않으면 됨)

		// 헤더(항상 BaseKey로 덮어쓰기)
		GEngine->AddOnScreenDebugMessage(
			BaseKey,
			DebugScreenDuration,
			FColor::White,
			FString::Printf(TEXT("[GachaDebug] %s  x%d  (%d results)"),
				*BannerID.ToString(), DrawCount, Results.Num())
		);

		// 결과 라인도 i번째는 항상 BaseKey+1+i로 덮어쓰기
		for (int32 i = 0; i < Results.Num(); ++i)
		{
			const FLinearColor LC = DebugRarityToColor(Results[i].Rarity);
			const FColor C = LC.ToFColor(true);

			GEngine->AddOnScreenDebugMessage(
				BaseKey + 1 + i,
				DebugScreenDuration,
				C,
				FString::Printf(TEXT("#%02d %s"), i + 1, *DebugResultToColoredString(Results[i]))
			);
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  시뮬레이션(테스트용)
// ─────────────────────────────────────────────────────────────────────────────

static int32 RarityToIndex(ELRGachaRarity R)
{
	switch (R)
	{
	case ELRGachaRarity::Common:    return 1;
	case ELRGachaRarity::Elite:     return 2;
	case ELRGachaRarity::Unique:    return 3;
	case ELRGachaRarity::Epic:      return 4;
	case ELRGachaRarity::Legendary: return 5;
	default:                        return 0;
	}
}

FString FLRGachaSimSummary::ToString() const
{
	auto Rate = [this](int32 C)
		{
			return (TotalPulls > 0) ? (100.0 * (double)C / (double)TotalPulls) : 0.0;
		};

	return FString::Printf(
		TEXT("Pulls=%d | 1★ %d(%.3f%%) 2★ %d(%.3f%%) 3★ %d(%.3f%%) 4★ %d(%.3f%%) 5★ %d(%.3f%%) | PityTriggered=%d | MaxNo5Streak=%d"),
		TotalPulls,
		Count1, Rate(Count1),
		Count2, Rate(Count2),
		Count3, Rate(Count3),
		Count4, Rate(Count4),
		Count5, Rate(Count5),
		PityTriggered,
		MaxNoLegendaryStreak
	);
}

bool ULRGachaSubsystem::Debug_SimulateBanner(
	FName BannerID,
	int32 TotalPulls,
	int32 Seed,
	FLRGachaSimSummary& OutSummary,
	bool bOverrideUsePity,
	bool bUsePityOverrideValue
)
{
	OutSummary = FLRGachaSimSummary{};
	OutSummary.TotalPulls = FMath::Max(0, TotalPulls);

	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("Debug_SimulateBanner 실패: 배너 없음 %s"), *BannerID.ToString());
		return false;
	}

	const bool bUsePitySim = bOverrideUsePity ? bUsePityOverrideValue : Banner.bUsePity;

	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		LR_WARN(TEXT("Debug_SimulateBanner 실패: Pool 비어있음 %s"), *BannerID.ToString());
		return false;
	}

	FRandomStream Stream(Seed);

	int32 SimPity = 0;
	int32 NoLegendaryStreak = 0;

	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, Banner.ItemType, Rates);

	auto PickRarity_Stream = [&Stream, &Rates](ELRGachaRarity& OutRarity) -> bool
		{
			float Total = 0.f;
			for (const auto& R : Rates)
			{
				Total += FMath::Max(0.f, R.Rate);
			}
			if (Total <= 0.f)
			{
				return false;
			}

			const float Rand = Stream.FRandRange(0.f, Total);
			float Acc = 0.f;
			for (const auto& R : Rates)
			{
				Acc += FMath::Max(0.f, R.Rate);
				if (Rand <= Acc)
				{
					OutRarity = R.Rarity;
					return true;
				}
			}
			return false;
		};

	auto PickUniform_Stream = [&Stream](const TArray<FLRGachaPoolRow>& In, FLRGachaPoolRow& Out) -> bool
		{
			if (In.Num() <= 0)
			{
				return false;
			}
			const int32 Idx = Stream.RandRange(0, In.Num() - 1);
			Out = In[Idx];
			return true;
		};

	for (int32 i = 0; i < OutSummary.TotalPulls; i++)
	{
		const bool bShouldPity = (bUsePitySim && (SimPity + 1) >= Banner.PityThreshold);

		FLRGachaPoolRow Picked;
		bool bOk = false;

		if (bShouldPity)
		{
			OutSummary.PityTriggered++;

			TArray<FLRGachaPoolRow> Filtered;
			for (const auto& Row : Pool)
			{
				if (Row.Rarity == Banner.PityGuaranteedRarity)
				{
					Filtered.Add(Row);
				}
			}
			bOk = PickUniform_Stream(Filtered, Picked);
		}
		else
		{
			ELRGachaRarity RolledRarity = ELRGachaRarity::Common;
			if (!PickRarity_Stream(RolledRarity))
			{
				LR_WARN(TEXT("Debug_SimulateBanner 실패: 등급 뽑기 실패"));
				return false;
			}

			TArray<FLRGachaPoolRow> Filtered;
			for (const auto& Row : Pool)
			{
				if (Row.Rarity == RolledRarity)
				{
					Filtered.Add(Row);
				}
			}
			bOk = PickUniform_Stream(Filtered, Picked);
		}

		if (!bOk)
		{
			LR_WARN(TEXT("Debug_SimulateBanner 실패: pick 실패 (banner=%s)"), *BannerID.ToString());
			return false;
		}

		const int32 Star = RarityToIndex(Picked.Rarity);
		if (Star == 1)      OutSummary.Count1++;
		else if (Star == 2) OutSummary.Count2++;
		else if (Star == 3) OutSummary.Count3++;
		else if (Star == 4) OutSummary.Count4++;
		else if (Star == 5) OutSummary.Count5++;

		if (bUsePitySim)
		{
			if (Picked.Rarity == ELRGachaRarity::Legendary)
			{
				SimPity = 0;
				NoLegendaryStreak = 0;
			}
			else
			{
				SimPity++;
				NoLegendaryStreak++;
				OutSummary.MaxNoLegendaryStreak = FMath::Max(OutSummary.MaxNoLegendaryStreak, NoLegendaryStreak);
			}
		}
	}

	return true;
}