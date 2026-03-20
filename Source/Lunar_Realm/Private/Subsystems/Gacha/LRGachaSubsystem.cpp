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
#include "Engine/Texture2D.h"

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
	if (RevealVisualDataTable.IsNull())
	{
		RevealVisualDataTable = TSoftObjectPtr<UDataTable>(
			FSoftObjectPath(TEXT("DataTable'/Game/DataTables/Gacha/DT_GachaRevealVisuals.DT_GachaRevealVisuals'"))
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

	// 마지막 선택 배너로도 저장
	SetLastShopBanner(InBannerID);
}

void ULRGachaSubsystem::ClearOpenShopOnLobbyReturn()
{
	bOpenShopOnLobbyReturn = false;
	PendingReturnShopBannerID = NAME_None;
}

bool ULRGachaSubsystem::ConsumePendingReturnShopBanner(FName& OutBannerID)
{
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

	FString MapName = LoadedWorld->GetMapName();
	const FString Prefix = LoadedWorld->StreamingLevelsPrefix;
	if (!Prefix.IsEmpty())
	{
		MapName.RemoveFromStart(Prefix);
	}

	if (!MapName.Equals(TEXT("Map_Lobby")))
	{
		return;
	}

	bOpenShopOnLobbyReturn = false;

	UGameInstance* GI = LoadedWorld->GetGameInstance();
	if (!GI) return;

	UUIManagerSubsystem* UISys = GI->GetSubsystem<UUIManagerSubsystem>();
	if (!UISys) return;

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
	LoadedRevealVisualDT = RevealVisualDataTable.IsNull() ? nullptr : Cast<UDataTable>(RevealVisualDataTable.LoadSynchronous());

	LR_INFO(
		TEXT("Gacha DT Loaded: Banner=%s Pool=%s Dup=%s Rate=%s Reveal=%s"),
		LoadedBannerDT ? TEXT("OK") : TEXT("NULL"),
		LoadedPoolDT ? TEXT("OK") : TEXT("NULL"),
		LoadedDupRewardDT ? TEXT("OK") : TEXT("NULL"),
		LoadedRarityRateDT ? TEXT("OK") : TEXT("NULL"),
		LoadedRevealVisualDT ? TEXT("OK") : TEXT("NULL")
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

bool ULRGachaSubsystem::GetRevealVisualRow(
	FName ItemID,
	ELRGachaItemType ItemType,
	FLRGachaRevealVisualRow& OutRow) const
{
	OutRow = FLRGachaRevealVisualRow();

	if (!LoadedRevealVisualDT)
	{
		return false;
	}

	const TArray<FName> RowNames = LoadedRevealVisualDT->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FLRGachaRevealVisualRow* Row =
			LoadedRevealVisualDT->FindRow<FLRGachaRevealVisualRow>(RowName, TEXT("RevealVisualLookup"));

		if (!Row)
		{
			continue;
		}

		if (Row->ItemID == ItemID && Row->ItemType == ItemType)
		{
			OutRow = *Row;
			return true;
		}
	}

	return false;
}

UTexture2D* ULRGachaSubsystem::GetResultSlotTexture(FName ItemID, ELRGachaItemType ItemType) const
{
	FLRGachaRevealVisualRow RevealRow;
	if (GetRevealVisualRow(ItemID, ItemType, RevealRow))
	{
		if (!RevealRow.ResultSlotTexture.IsNull())
		{
			return RevealRow.ResultSlotTexture.LoadSynchronous();
		}
	}

	return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  리빌 화면 표시 데이터 구성
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::BuildRevealPresentationData(
	const FLRGachaResult& Result,
	FLRGachaRevealPresentationData& OutData) const
{
	OutData = FLRGachaRevealPresentationData();
	OutData.ItemID = Result.ItemID;
	OutData.ItemType = Result.ItemType;
	OutData.Rarity = Result.Rarity;

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	UGameDataSubsystem* GameDataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!GameDataSys)
	{
		return false;
	}

	// 1) 이름만 기본 정적 데이터에서 가져옴
	if (Result.ItemType == ELRGachaItemType::Hero)
	{
		const FCharacterStaticData& CharData = GameDataSys->GetCharacterStaticData(Result.ItemID);
		OutData.DisplayName = FText::FromString(CharData.CharacterName);
	}
	else
	{
		const FEquipmentStaticData& EquipData = GameDataSys->GetEquipmentStaticData(Result.ItemID);
		OutData.DisplayName = FText::FromString(EquipData.EquipmentName);
	}

	// 2) 가챠 리빌 전용 DT에서만 이미지/영상 가져옴
	FLRGachaRevealVisualRow RevealRow;
	if (!GetRevealVisualRow(Result.ItemID, Result.ItemType, RevealRow))
	{
		// 리빌 전용 데이터가 없으면 이름만 보여주고 false 반환
		return false;
	}

	if (!RevealRow.BackgroundTexture.IsNull())
	{
		OutData.BackgroundTexture = RevealRow.BackgroundTexture.LoadSynchronous();
	}

	if (!RevealRow.RevealTexture.IsNull())
	{
		OutData.MainTexture = RevealRow.RevealTexture.LoadSynchronous();
	}

	OutData.bUseVideo = RevealRow.bUseVideo;

	if (!RevealRow.RevealSFX.IsNull())
	{
		OutData.RevealSound = RevealRow.RevealSFX.LoadSynchronous();
	}

	if (!RevealRow.ColorRevealSFX.IsNull())
	{
		OutData.ColorRevealSound = RevealRow.ColorRevealSFX.LoadSynchronous();
	}

	if (RevealRow.bUseVideo && !RevealRow.RevealVideoSource.IsNull())
	{
		OutData.VideoSource = RevealRow.RevealVideoSource.LoadSynchronous();
	}

	return (OutData.MainTexture != nullptr);
}

// ─────────────────────────────────────────────────────────────────────────────
//  희귀도/풀 선택 로직
// ─────────────────────────────────────────────────────────────────────────────

bool ULRGachaSubsystem::PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const
{
	OutRarity = ELRGachaRarity::N;

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

	if (bTxnInProgress)
	{
		LR_WARN(TEXT("BeginDrawTransaction 차단: 이미 트랜잭션 진행 중"));
		return false;
	}

	if (S->GachaPendingTransactions.Num() > 0)
	{
		LR_WARN(TEXT("BeginDrawTransaction 차단: Pending 트랜잭션이 남아있음"));
		return false;
	}

	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: 배너 없음 %s"), *BannerID.ToString());
		return false;
	}

	int32 NeedCost = 0;
	if (!CanDraw(BannerID, DrawCount, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: 재화 부족"));
		return false;
	}

	if (!SpendCurrency(Banner.CostCurrencyType, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction 실패: SpendCurrency 실패"));
		return false;
	}

	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		AddCurrency(Banner.CostCurrencyType, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction 실패: Pool 비어있음 → 환불 처리"));
		return false;
	}

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

	const int32 PrevPity = GetPityCount(BannerID);
	int32 WorkingPity = PrevPity;

	if (!RollResults_NoApply(Banner, Pool, DrawCount, WorkingPity, OutResults))
	{
		AddCurrency(Banner.CostCurrencyType, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction 실패: 롤링 실패 → 환불 처리"));
		return false;
	}

	if (Banner.bUsePity)
	{
		S->GachaPityCounterMap.FindOrAdd(BannerID) = WorkingPity;
		OnPityChanged.Broadcast(BannerID, WorkingPity);
	}

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

	bTxnInProgress = true;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);
	OnGachaFinished.Broadcast(BannerID, OutResults);

	OutTxnId = Pending.TxnId;

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

	const FName BannerIDCopy = PendingPtr->BannerID;
	TArray<FLRGachaResult> ResultsCopy = PendingPtr->Results;

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

	S->GachaPendingTransactions.Remove(TxnId);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}

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

	AddCurrency(CostTypeCopy, CostAmountCopy);

	S->GachaPityCounterMap.FindOrAdd(BannerIDCopy) = PrevPityCopy;
	OnPityChanged.Broadcast(BannerIDCopy, PrevPityCopy);

	S->GachaPendingTransactions.Remove(TxnId);

	if (SaveGameSubsystem)
	{
		SaveGameSubsystem->UpdateLastSavedTimeAndSave();
	}

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

	TSet<FName> NewlyObtainedInThisTxn;

	for (int32 i = 0; i < DrawCount; i++)
	{
		const bool bShouldPity = (Banner.bUsePity && (InOutPityCounter + 1) >= Banner.PityThreshold);

		FLRGachaPoolRow Picked;
		bool bPickedOk = false;

		if (bShouldPity)
		{
			bPickedOk = PickOneFromPoolByRarity(Pool, Banner.PityGuaranteedRarity, Picked);
		}
		else
		{
			ELRGachaRarity RolledRarity = ELRGachaRarity::N;
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
			Result.bIsNew = false;
			Result.bConvertedToGold = true;
			Result.ConvertedGoldAmount = GetDuplicateGold(Result.Rarity);
		}

		if (Banner.bUsePity)
		{
			if (Result.Rarity == ELRGachaRarity::UR)
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
	case ELRGachaRarity::N:   return FLinearColor(0.65f, 0.65f, 0.65f, 1.0f);
	case ELRGachaRarity::R:   return FLinearColor(0.55f, 1.00f, 0.35f, 1.0f);
	case ELRGachaRarity::SR:  return FLinearColor(0.15f, 0.45f, 1.00f, 1.0f);
	case ELRGachaRarity::SSR: return FLinearColor(0.60f, 0.20f, 0.90f, 1.0f);
	case ELRGachaRarity::UR:  return FLinearColor(1.00f, 0.78f, 0.10f, 1.0f);
	default:                  return FLinearColor::White;
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
	case ELRGachaRarity::N:   ColorName = TEXT("Gray");      break;
	case ELRGachaRarity::R:   ColorName = TEXT("LimeGreen"); break;
	case ELRGachaRarity::SR:  ColorName = TEXT("Blue");      break;
	case ELRGachaRarity::SSR: ColorName = TEXT("Purple");    break;
	case ELRGachaRarity::UR:  ColorName = TEXT("Gold");      break;
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

	if (bDebugPrintToScreen && GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();

		const int32 BaseKey = 9000;

		GEngine->AddOnScreenDebugMessage(
			BaseKey,
			DebugScreenDuration,
			FColor::White,
			FString::Printf(TEXT("[GachaDebug] %s  x%d  (%d results)"),
				*BannerID.ToString(), DrawCount, Results.Num())
		);

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
	case ELRGachaRarity::N:   return 1;
	case ELRGachaRarity::R:   return 2;
	case ELRGachaRarity::SR:  return 3;
	case ELRGachaRarity::SSR: return 4;
	case ELRGachaRarity::UR:  return 5;
	default:                  return 0;
	}
}

FString FLRGachaSimSummary::ToString() const
{
	auto Rate = [this](int32 C)
		{
			return (TotalPulls > 0) ? (100.0 * (double)C / (double)TotalPulls) : 0.0;
		};

	return FString::Printf(
		TEXT("Pulls=%d | N %d(%.3f%%) R %d(%.3f%%) SR %d(%.3f%%) SSR %d(%.3f%%) UR %d(%.3f%%) | PityTriggered=%d | MaxNoURStreak=%d"),
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
			ELRGachaRarity RolledRarity = ELRGachaRarity::N;
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
			if (Picked.Rarity == ELRGachaRarity::UR)
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

bool ULRGachaSubsystem::PeekPendingReturnShopBanner(FName& OutBannerID) const
{
	if (PendingReturnShopBannerID.IsNone())
	{
		return false;
	}

	OutBannerID = PendingReturnShopBannerID;
	return true;
}

void ULRGachaSubsystem::ClearPendingReturnShopBanner()
{
	PendingReturnShopBannerID = NAME_None;
}

void ULRGachaSubsystem::SetLastShopBanner(FName InBannerID)
{
	if (!InBannerID.IsNone())
	{
		LastShopBannerID = InBannerID;
	}
}

FName ULRGachaSubsystem::GetLastShopBanner() const
{
	return LastShopBannerID;
}