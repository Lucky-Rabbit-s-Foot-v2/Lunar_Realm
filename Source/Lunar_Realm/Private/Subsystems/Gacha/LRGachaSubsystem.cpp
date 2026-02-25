// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "SaveGame/LRSaveGame.h"
#include "System/LoggingSystem.h"

#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

// ───────────────── UGameInstanceSubsystem 구현 ─────────────────

void ULRGachaSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// DataTable 경로 기본값 (에디터에서 지정 안 했을 때)
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

	if (UGameInstance* GI = GetGameInstance())
	{
		CurrencySubsystem = GI->GetSubsystem<UCurrencySubsystem>();
		SaveGameSubsystem = GI->GetSubsystem<USaveGameSubsystem>();
	}

	// DataTable 로드
	LoadDataTables();

	LR_INFO(TEXT("LRGachaSubsystem Initialized"));

	// SaveGame에 Pending 트랜잭션이 남아있으면, 상태를 PendingReveal로 세팅
	if (ULRSaveGame* S = GetSaveGame())
	{
		if (S->GachaPendingTransactions.Num() > 0)
		{
			bTxnInProgress = true;
			OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);
		}
	}
}

// ───────────────── DataTable 로드/조회 ─────────────────

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

// ───────────────── 재화 API ─────────────────

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

// ───────────────── 천장(Pity) 처리 ─────────────────

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

// ───────────────── DataTable 조회 ─────────────────

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

void ULRGachaSubsystem::GetRarityRateRowsForBanner(FName BannerID, ELRGachaItemType ItemType, TArray<FLRGachaRarityRateRow>& OutRows) const
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

// ───────────────── 희귀도/풀 선택 ─────────────────

bool ULRGachaSubsystem::PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const
{
	OutRarity = ELRGachaRarity::Common;

	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, ItemType, Rates);

	if (Rates.Num() <= 0)
	{
		return false;
	}

	// 전체 합
	float Total = 0.0f;
	for (const auto& R : Rates)
	{
		Total += FMath::Max(0.0f, R.Rate);
	}
	if (Total <= 0.0f)
	{
		return false;
	}

	// 0 ~ Total 랜덤값을 누적합으로 맞춰서 등급 결정
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

bool ULRGachaSubsystem::PickOneFromPoolByRarity(const TArray<FLRGachaPoolRow>& Pool, ELRGachaRarity TargetRarity, FLRGachaPoolRow& OutPicked) const
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

// ───────────────── 중복 보상 ─────────────────

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

// ───────────────── 뽑기 가능 여부 ─────────────────

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

// ───────────────── 트랜잭션: Begin ─────────────────

bool ULRGachaSubsystem::BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults)
{
	OutTxnId.Invalidate();
	OutResults.Empty();

	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		LR_WARN(TEXT("BeginDrawTransaction failed : SaveGame null"));
		return false;
	}

	// 연타/중복 실행 방지(락)
	if (bTxnInProgress)
	{
		LR_WARN(TEXT("BeginDrawTransaction blocked : transaction already in progress"));
		return false;
	}

	// Pending이 남아있으면 새 트랜잭션 시작 금지
	if (S->GachaPendingTransactions.Num() > 0)
	{
		LR_WARN(TEXT("BeginDrawTransaction blocked: Pending transaction exists"));
		return false;
	}

	// 배너 로드
	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("BeginDrawTransaction failed: Banner not found %s"), *BannerID.ToString());
		return false;
	}

	// 비용 체크
	int32 NeedCost = 0;
	if (!CanDraw(BannerID, DrawCount, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction failed: not enough currency"));
		return false;
	}

	// 비용 차감(예약)
	if (!SpendCurrency(Banner.CostCurrencyType, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction failed: spend failed"));
		return false;
	}

	// 뽑기 풀 로드
	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		// 차감했는데 풀 없음 → 환불
		AddCurrency(Banner.CostCurrencyType, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction failed: Pool empty. Refunded"));
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
			AddCurrency(Banner.CostCurrencyType, NeedCost); // 환불
			LR_WARN(TEXT("BeginDrawTransaction failed: RarityRate missing/invalid. Refunded"));
			return false;
		}
	}

	// 천장 계산용 작업용 pity
	const int32 PrevPity = GetPityCount(BannerID);
	int32 WorkingPity = PrevPity;

	// 결과 롤링(지급X)
	if (!RollResults_NoApply(Banner, Pool, DrawCount, WorkingPity, OutResults))
	{
		AddCurrency(Banner.CostCurrencyType, NeedCost); // 환불
		LR_WARN(TEXT("BeginDrawTransaction failed: roll failed. Refunded"));
		return false;
	}

	// 천장 카운터를 "결과 확정 시점"에 반영
	if (Banner.bUsePity)
	{
		S->GachaPityCounterMap.FindOrAdd(BannerID) = WorkingPity;
		OnPityChanged.Broadcast(BannerID, WorkingPity);
	}

	// Pending 트랜잭션 저장 (튕김 복구 핵심)
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

	// 락 ON + 상태 브로드캐스트
	bTxnInProgress = true;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);

	// UI 연출 시작 이벤트 (원하면 여기 구독해서 바로 리빌 열 수도 있음)
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

// ───────────────── 트랜잭션: Commit ─────────────────

bool ULRGachaSubsystem::CommitTransaction(const FGuid& TxnId)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return false;
	}

	// Pending 트랜잭션 찾기
	FLRGachaPendingTransaction* PendingPtr = S->GachaPendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CommitTransaction failed: Pending not found"));
		return false;
	}

	// Remove 전에 필요한 값 복사(안전)
	const FName BannerIDCopy = PendingPtr->BannerID;
	TArray<FLRGachaResult> ResultsCopy = PendingPtr->Results;

	// 실제 지급(컬렉션 반영/중복 골드 지급)
	for (const FLRGachaResult& Result : ResultsCopy)
	{
		bool bWasNew = false;
		if (!TryAddToCollection(Result, bWasNew))
		{
			LR_WARN(TEXT("CommitTransaction: TryAddToCollection failed"));
			continue;
		}

		// 중복이면 골드 지급
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

// ───────────────── 트랜잭션: Cancel ─────────────────

bool ULRGachaSubsystem::CancelTransaction(const FGuid& TxnId)
{
	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return false;
	}

	// Pending 찾기
	FLRGachaPendingTransaction* PendingPtr = S->GachaPendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CancelTransaction failed: pending not found"));
		return false;
	}

	// Remove 전에 복사(안전)
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

	LR_INFO(
		TEXT("CancelTransaction OK: %s Txn=%s Refunded=%d"),
		*BannerIDCopy.ToString(),
		*TxnId.ToString(),
		CostAmountCopy
	);
	return true;
}

// ───────────────── 결과 롤링(지급X) ─────────────────

bool ULRGachaSubsystem::RollResults_NoApply(
	const FLRGachaBannerRow& Banner,
	const TArray<FLRGachaPoolRow>& Pool,
	int32 DrawCount,
	int32& InOutPityCounter,
	TArray<FLRGachaResult>& OutResults
)
{
	OutResults.Empty();

	// 컬렉션 시스템(보유 여부 판단용)
	UCollectionSubsystem* CollectionSys = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	if (!CollectionSys)
	{
		LR_WARN(TEXT("RollResults_NoApply: CollectionSubsystem not found"));
		return false;
	}

	TSet<FName> NewlyObtainedInThisTxn; // 이번 트랜잭션에서 새로 뽑힌 것

	for (int32 i = 0; i < DrawCount; i++)
	{
		const bool bShouldPity = (Banner.bUsePity && (InOutPityCounter + 1) >= Banner.PityThreshold);

		FLRGachaPoolRow Picked;
		bool bPickedOk = false;

		if (bShouldPity)
		{
			// 천장 발동이면 확정 등급에서 뽑기
			bPickedOk = PickOneFromPoolByRarity(Pool, Banner.PityGuaranteedRarity, Picked);
		}
		else
		{
			// 천장 아니면: “등급 확률” → 해당 등급 풀에서 랜덤
			ELRGachaRarity RolledRarity = ELRGachaRarity::Common;
			const bool bHasRate = PickRarityByRates(Banner.BannerID, Banner.ItemType, RolledRarity);

			if (!bHasRate)
			{
				LR_WARN(TEXT("RollResults_NoApply: PickRarityByRates failed (banner=%s)"), *Banner.BannerID.ToString());
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
			LR_WARN(TEXT("RollResults_NoApply: pick failed"));
			return false;
		}

		// Result 구성
		FLRGachaResult Result;
		Result.ItemType = Banner.ItemType;
		Result.ItemID = Picked.ItemID;
		Result.Rarity = Picked.Rarity;

		// "지급 전" 기준으로 New/중복 판정
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
			// 진짜 신규
			Result.bIsNew = true;
			NewlyObtainedInThisTxn.Add(Result.ItemID);
		}
		else
		{
			// 중복이면 "골드 전환" 정보 미리 계산
			Result.bIsNew = false;
			Result.bConvertedToGold = true;
			Result.ConvertedGoldAmount = GetDuplicateGold(Result.Rarity);
		}

		// 천장 카운터 갱신
		if (Banner.bUsePity)
		{
			if (Result.Rarity == ELRGachaRarity::Legendary)
			{
				InOutPityCounter = 0; // 전설이면 리셋
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

// ───────────────── Pending 트랜잭션 조회 ─────────────────

bool ULRGachaSubsystem::GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const
{
	OutPending = FLRGachaPendingTransaction();

	ULRSaveGame* S = GetSaveGame();
	if (!S)
	{
		return false;
	}
	if (S->GachaPendingTransactions.Num() <= 0)
	{
		return false;
	}

	// 하나만 써도 되므로 첫 번째 반환
	for (const auto& Pair : S->GachaPendingTransactions)
	{
		OutPending = Pair.Value;
		return true;
	}
	return false;
}

// ───────────────── UI 편의 함수 ─────────────────

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
		LR_WARN(TEXT("[Gacha] BeginDrawBySelection FAILED Banner=%s Count=%d"), *BannerID.ToString(), DrawCount);
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

// ───────────────── Pending Reveal (리빌 맵 연동) ─────────────────

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

	// 상태 리셋
	PendingResults.Reset();
	PendingBannerID = NAME_None;
	PendingTxnId.Invalidate();
	bHasPendingReveal = false;

	return true;
}

// ───────────────── Debug 출력 ─────────────────

FLinearColor ULRGachaSubsystem::DebugRarityToColor(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);      // Gray
	case ELRGachaRarity::Elite:     return FLinearColor(0.20f, 0.55f, 1.00f, 1.0f);   // Blue
	case ELRGachaRarity::Unique:    return FLinearColor(1.00f, 0.25f, 0.25f, 1.0f);   // Red
	case ELRGachaRarity::Epic:      return FLinearColor(0.6f, 0.2f, 0.8f, 1.0f);      // Purple
	case ELRGachaRarity::Legendary: return FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);      // Gold
	default:                        return FLinearColor::White;
	}
}

FString ULRGachaSubsystem::DebugResultToColoredString(const FLRGachaResult& Result) const
{
	const UEnum* ItemTypeEnum = StaticEnum<ELRGachaItemType>();
	const UEnum* RarityEnum = StaticEnum<ELRGachaRarity>();

	const FString ItemTypeStr = ItemTypeEnum
		? ItemTypeEnum->GetNameStringByValue((int64)Result.ItemType)
		: TEXT("UnknownType");

	const FString RarityStr = RarityEnum
		? RarityEnum->GetNameStringByValue((int64)Result.Rarity)
		: TEXT("UnknownRarity");

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
	// 로그
	if (bDebugPrintToLog)
	{
		LR_INFO(
			TEXT("[GachaDebug] Banner=%s Count=%d Txn=%s Results=%d"),
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

	// 화면
	if (bDebugPrintToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			DebugScreenDuration,
			FColor::White,
			FString::Printf(TEXT("[GachaDebug] %s  x%d  (%d results)"), *BannerID.ToString(), DrawCount, Results.Num())
		);

		for (int32 i = 0; i < Results.Num(); ++i)
		{
			const FLinearColor LC = DebugRarityToColor(Results[i].Rarity);
			const FColor C = LC.ToFColor(true);

			GEngine->AddOnScreenDebugMessage(
				-1,
				DebugScreenDuration,
				C,
				FString::Printf(TEXT("#%02d %s"), i + 1, *DebugResultToColoredString(Results[i]))
			);
		}
	}
}

// ───────────────── 시뮬레이션 ─────────────────

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

	// 배너/풀 로드
	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("Debug_SimulateBanner: Banner not found: %s"), *BannerID.ToString());
		return false;
	}

	const bool bUsePitySim = bOverrideUsePity ? bUsePityOverrideValue : Banner.bUsePity;

	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		LR_WARN(TEXT("Debug_SimulateBanner: Pool empty: %s"), *BannerID.ToString());
		return false;
	}

	FRandomStream Stream(Seed);

	// 실제 저장된 천장 카운트는 건드리면 안 되므로, 로컬 변수 사용
	int32 SimPity = 0;
	int32 NoLegendaryStreak = 0;

	// 등급 확률 Row 캐시
	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, Banner.ItemType, Rates);

	// 등급 뽑기 (RandomStream 버전)
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

	// 풀에서 균등 랜덤 1개 선택 (RandomStream 버전)
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

			// 특정 등급만 필터해서 그 안에서 uniform
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
				LR_WARN(TEXT("Debug_SimulateBanner: rate pick failed"));
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
			LR_WARN(TEXT("Debug_SimulateBanner: pick failed (banner=%s)"), *BannerID.ToString());
			return false;
		}

		// 통계 집계
		const int32 Star = RarityToIndex(Picked.Rarity);
		if (Star == 1)      OutSummary.Count1++;
		else if (Star == 2) OutSummary.Count2++;
		else if (Star == 3) OutSummary.Count3++;
		else if (Star == 4) OutSummary.Count4++;
		else if (Star == 5) OutSummary.Count5++;

		// 천장 카운터 갱신 (Legendary면 초기화)
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