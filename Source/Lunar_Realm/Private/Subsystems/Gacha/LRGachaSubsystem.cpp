// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "SaveGame/Gacha/LRGachaSaveGame.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "System/LoggingSystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"


void ULRGachaSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// DT경로 하드코딩 기본값 주입
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

	// 저장 로드/생성
	LoadOrCreateSave();

	// DataTable 로드
	LoadDataTables();

	LR_INFO(TEXT("LRGachaSubsystem Initialized"));
}

void ULRGachaSubsystem::LoadOrCreateSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		GachaSave = Cast<ULRGachaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	}
	else
	{
		GachaSave = Cast<ULRGachaSaveGame>(UGameplayStatics::CreateSaveGameObject(ULRGachaSaveGame::StaticClass()));

		// 테스트용 초기 재화
		GachaSave->CurrencyMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Currency.Gold")), 1000);
		GachaSave->CurrencyMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Currency.Ticket.Crescent")), 1000);
		GachaSave->CurrencyMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Currency.Ticket.FullMoon")), 1000);

		Save();
	}

	if (!ensureMsgf(GachaSave, TEXT("GachaSave is invalid")))
	{
		return;
	}
}

void ULRGachaSubsystem::Save()
{
	if (!GachaSave) return;

	UGameplayStatics::SaveGameToSlot(GachaSave, SaveSlotName, UserIndex);
}

void ULRGachaSubsystem::LoadDataTables()
{
	// DT를 에디터에서 지정해야함(SoftObjectPtr)
	LoadedBannerDT = BannerDataTable.IsNull() ? nullptr : Cast<UDataTable>(BannerDataTable.LoadSynchronous());
	LoadedPoolDT = PoolDataTable.IsNull() ? nullptr : Cast<UDataTable>(PoolDataTable.LoadSynchronous());
	LoadedDupRewardDT = DuplicateRewardDataTable.IsNull() ? nullptr : Cast<UDataTable>(DuplicateRewardDataTable.LoadSynchronous());
	LoadedRarityRateDT = RarityRateDataTable.IsNull() ? nullptr : Cast<UDataTable>(RarityRateDataTable.LoadSynchronous());

	LR_INFO(TEXT("Gacha DT Loaded: Banner=%s Pool=%s Dup=%s Rate=%s"),
		LoadedBannerDT ? TEXT("OK") : TEXT("NULL"),
		LoadedPoolDT ? TEXT("OK") : TEXT("NULL"),
		LoadedDupRewardDT ? TEXT("OK") : TEXT("NULL"),
		LoadedRarityRateDT ? TEXT("OK") : TEXT("NULL"));
}

// Currency ==================================================================
int32 ULRGachaSubsystem::GetCurrency(FGameplayTag CurrencyTag) const
{
	if (!GachaSave) return 0;

	if (const int32* Found = GachaSave->CurrencyMap.Find(CurrencyTag))
	{
		return *Found;
	}
	return 0;
}

void ULRGachaSubsystem::AddCurrency(FGameplayTag CurrencyTag, int32 Delta)
{
	if (!GachaSave) return;

	int32& Value = GachaSave->CurrencyMap.FindOrAdd(CurrencyTag);
	Value = FMath::Max(0, Value + Delta);

	Save();
	OnCurrencyChanged.Broadcast(CurrencyTag, Value);
}

bool ULRGachaSubsystem::SpendCurrency(FGameplayTag CurrencyTag, int32 Cost)
{
	if (!GachaSave) return false;

	int32 Current = GetCurrency(CurrencyTag);
	if (Current < Cost) return false;

	// AddCurrency 안에서 Save + Broadcast까지 처리하므로 여기선 -만 해주면 됨
	AddCurrency(CurrencyTag, -Cost);	
	return true;
}

// Pity(천장) ==========================================================
int32 ULRGachaSubsystem::GetPityCount(FName BannerID) const
{
	if (!GachaSave) return 0;
	if (const int32* Found = GachaSave->PityCounterMap.Find(BannerID))
	{
		return *Found;
	}
	return 0;
}

void ULRGachaSubsystem::IncrementPity(FName BannerID)
{
	if (!GachaSave) return;

	int32& Value = GachaSave->PityCounterMap.FindOrAdd(BannerID);
	Value++;

	Save();
	OnPityChanged.Broadcast(BannerID, Value);
}

void ULRGachaSubsystem::ResetPity(FName BannerID)
{
	if (!GachaSave) return;

	GachaSave->PityCounterMap.FindOrAdd(BannerID) = 0;

	Save();
	OnPityChanged.Broadcast(BannerID, 0);
}

// 데이터 테이블 조회 ============================================================================================
bool ULRGachaSubsystem::GetBannerRow(FName BannerID, FLRGachaBannerRow& OutRow) const
{
	if (!LoadedBannerDT) return false;

	// RowName을 BannerID와 동일하게 쓰기
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
	if (!LoadedPoolDT) return;

	// Pool DT는 "RowName"이 무엇이든 상관없이, Row 내부 BannerID로 필터링
	TArray<FName> RowNames = LoadedPoolDT->GetRowNames();
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
	if (!LoadedRarityRateDT) return;

	const TArray<FName> RowNames = LoadedRarityRateDT->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FLRGachaRarityRateRow* Row = LoadedRarityRateDT->FindRow<FLRGachaRarityRateRow>(RowName, TEXT("RarityRateLookup"));
		if (!Row) continue;

		if (Row->BannerID == BannerID && Row->ItemType == ItemType)
		{
			OutRows.Add(*Row);
		}
	}
}
	
// 희귀도 목록 =======================================================================================================
bool ULRGachaSubsystem::PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const
{
	OutRarity = ELRGachaRarity::Common;

	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, ItemType, Rates);

	// 확률 Row가 없으면 실패
	if (Rates.Num() <= 0) return false;

	// 전체 합
	float Total = 0.0f;
	for (const auto& R : Rates)
	{
		Total += FMath::Max(0.0f, R.Rate);
	}
	if (Total <= 0.0f) return false;

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

// Pool Pick ========================================================================================================
bool ULRGachaSubsystem::PickOneFromPoolUniform(const TArray<FLRGachaPoolRow>& Pool, FLRGachaPoolRow& OutPicked) const
{
	if (Pool.Num() <= 0) return false;

	const int32 Index = FMath::RandRange(0, Pool.Num() - 1);
	OutPicked = Pool[Index];
	return true;
}


// 특정 등급만 강제해서 뽑기(천장용)
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
	return PickOneFromPoolUniform(Filtered, OutPicked);
}

// 중복보상 골드량 조회
int32 ULRGachaSubsystem::GetDuplicateGold(ELRGachaRarity Rarity) const
{
	if(!LoadedDupRewardDT) return 0;

	// RowName을 "Common/Elite/..." 키로 맞춘다는 규칙
	const TCHAR* RowKey = TEXT("Common");
	switch (Rarity)
	{
	case ELRGachaRarity::Common: RowKey = TEXT("Common"); break;
	case ELRGachaRarity::Elite: RowKey = TEXT("Elite"); break;
	case ELRGachaRarity::Unique: RowKey = TEXT("Unique"); break;
	case ELRGachaRarity::Epic: RowKey = TEXT("Epic"); break;
	case ELRGachaRarity::Legendary: RowKey = TEXT("Legendary"); break;
	}

	if (FLRGachaDuplicateRewardRow* Row = LoadedDupRewardDT->FindRow<FLRGachaDuplicateRewardRow>(FName(RowKey), TEXT("DupRewardLookup")))
	{
		return Row->GoldAmount;
	}
	return 0;
}


// 태그 헬퍼(골드)
FGameplayTag ULRGachaSubsystem::GetGoldTag() const
{
	return FGameplayTag::RequestGameplayTag(TEXT("Currency.Gold"));
}

// 실제 획득 처리(컬렉션에 추가 or 중복이면 골드) =====================================================
void ULRGachaSubsystem::ApplyResultToCollection(FLRGachaResult& InOutResult)
{
	UCollectionSubsystem* CollectionSys = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	if (!CollectionSys)
	{
		LR_WARN(TEXT("CollectionSubsystem not found"));
		return;
	}

	if (InOutResult.ItemType == ELRGachaItemType::Hero)
	{
		const bool bOwned = CollectionSys->HasCharacter(InOutResult.ItemID);
		if (!bOwned)
		{
			CollectionSys->AddCharacter(InOutResult.ItemID, 1);
			InOutResult.bIsNew = true;
		}
		else
		{
			// 중복->골드 변환
			const int32 Gold = GetDuplicateGold(InOutResult.Rarity);
			AddCurrency(GetGoldTag(), Gold);

			InOutResult.bIsNew = false;
			InOutResult.bConvertedToGold = true;
			InOutResult.ConvertedGoldAmount = Gold;
		}
	}
	else // Equipment
	{
		const bool bOwned = CollectionSys->HasEquipment(InOutResult.ItemID);
		if (!bOwned)
		{
			CollectionSys->AddEquipment(InOutResult.ItemID, 1);
			InOutResult.bIsNew = true;
		}
		else
		{
			const int32 Gold = GetDuplicateGold(InOutResult.Rarity);
			AddCurrency(GetGoldTag(), Gold);

			InOutResult.bIsNew = false;
			InOutResult.bConvertedToGold = true;
			InOutResult.ConvertedGoldAmount = Gold;
		}
	}
}

// CanDraw / Draw (단순 뽑기) =====================================================================
bool ULRGachaSubsystem::CanDraw(FName BannerID, int32 DrawCount, int32& OutNeedCost) const
{
	OutNeedCost = 0;

	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner)) return false;

	// 1회/10회 외 값이 들어오면 1회 비용 * DrawCount로 처리
	if (DrawCount == 1) OutNeedCost = Banner.CostSingle;
	else if (DrawCount == 10) OutNeedCost = Banner.CostTen;
	else OutNeedCost = Banner.CostSingle * DrawCount;

	return GetCurrency(Banner.CostCurrencyTag) >= OutNeedCost;	
}

// 실제 뽑기 실행(1회/10회)
bool ULRGachaSubsystem::Draw(FName BannerID, int32 DrawCount, TArray<FLRGachaResult>& OutResults)
{
	OutResults.Empty();

	FLRGachaBannerRow Banner;
	if (!GetBannerRow(BannerID, Banner))
	{
		LR_WARN(TEXT("Banner not found: %s"), *BannerID.ToString());
		return false;
	}

	int32 NeedCost = 0;
	if (!CanDraw(BannerID, DrawCount, NeedCost))
	{
		LR_WARN(TEXT("Not enough currency for draw"));
		return false;
	}

	// 비용 차감
	if (!SpendCurrency(Banner.CostCurrencyTag, NeedCost))
	{
		return false;
	}

	// 배너 풀 로드
	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		LR_WARN(TEXT("Pool empty for banner: %s"), *BannerID.ToString());
		return false;
	}

	// 확률 테이블 검증: 없거나 잘못되면 뽑기 불가 -> 비용 환불 후 종료
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
			// 비용 환불
			AddCurrency(Banner.CostCurrencyTag, NeedCost);

			LR_WARN(TEXT("Draw failed: RarityRate missing/invalid for banner: %s -> Refunded"),
				*BannerID.ToString());
			return false;
		}
	}

	// DrawCount 만큼 반복
	for (int32 i = 0; i < DrawCount; i++)
	{
		FLRGachaResult Result;
		Result.ItemType = Banner.ItemType;

		// 천장처리(보름달만 true)
		// 천장체크(현재 Pity + 1이 threshold면 확정)
		const int32 CurrentPity = GetPityCount(BannerID);
		const bool bShouldPity = (Banner.bUsePity && (CurrentPity + 1) >= Banner.PityThreshold);

		FLRGachaPoolRow Picked;
		bool bPickedOk = false;

		if (bShouldPity)
		{
			bPickedOk = PickOneFromPoolByRarity(Pool, Banner.PityGuaranteedRarity, Picked);
		}
		else
		{
			// 등급 확률로만 뽑기
			ELRGachaRarity RolledRarity = ELRGachaRarity::Common;
			const bool bHasRate = PickRarityByRates(BannerID, Banner.ItemType, RolledRarity);

			// 위에서 확률 테이블 검증을 했으니 여기서 false면 비정상
			if (!bHasRate)
			{
				LR_WARN(TEXT("PickRarityByRates failed unexpectedly (banner=%s)"), *BannerID.ToString());
				continue;
			}

			TArray<FLRGachaPoolRow> Filtered;
			for (const auto& Row : Pool)
			{
				if (Row.Rarity == RolledRarity)
				{
					Filtered.Add(Row);
				}
			}

			bPickedOk = PickOneFromPoolUniform(Filtered, Picked);
		}


		if (!bPickedOk)
		{
			LR_WARN(TEXT("Pick failed"));
			continue;
		}

		Result.ItemID = Picked.ItemID;
		Result.Rarity = Picked.Rarity;

		// 컬렉션 적용(중복이면 골드)
		ApplyResultToCollection(Result);

		// 천장 카운터 갱신 규칙
		// Legendary 뽑으면 Reset
		// 아니면 Increment(보름달 배너일 때만)
		if (Banner.bUsePity)
		{
			if (Result.Rarity == ELRGachaRarity::Legendary)
			{
				ResetPity(BannerID);
			}
			else
			{
				IncrementPity(BannerID);
			}
		}
		OutResults.Add(Result);
	}

	// 뽑기 완료 이벤트(연출/UI가 구독)
	OnGachaFinished.Broadcast(BannerID, OutResults);

	return true;	
}

// 트랜잭션 시작 : 비용 차감 + 결과 확정/저장 + (지급은 아직!)
bool ULRGachaSubsystem::BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults)
{
	OutTxnId.Invalidate();
	OutResults.Empty();

	// 저장 데이터가 없으면 진행 불가
	if (!GachaSave)
	{
		LR_WARN(TEXT("BeginDrawTransaction failed : GachaSave null"));
		return false;
	}
	
	// 연타/중복 실행 방지(락)
	// 모바일에서 버튼 연타하면 2번 이상 호출되는 사고가 잦아서 방어
	if (bTxnInProgress)
	{
		LR_WARN(TEXT("BeginDrawTransaction blocked : transaction already in progress"));
		return false;
	}

	// Pending이 남아있으면 새 트랜잭션 시작 금지
	// Pending이 있다는 건 “이전 뽑기가 아직 복구/처리되지 않았다”는 뜻
	if (GachaSave->PendingTransactions.Num() > 0)
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
	// 여기서 재화를 먼저 차감하고, 이후 오류나면 환불(AddCurrency)로 되돌린다
	if (!SpendCurrency(Banner.CostCurrencyTag, NeedCost))
	{
		LR_WARN(TEXT("BeginDrawTransaction failed: spend failed"));
		return false;
	}

	// 뽑기 풀 로드
	TArray<FLRGachaPoolRow> Pool;
	GetPoolRowsForBanner(BannerID, Pool);
	if (Pool.Num() <= 0)
	{
		// 차감했는데 풀 없음이면 환불해야함
		AddCurrency(Banner.CostCurrencyTag, NeedCost);
		LR_WARN(TEXT("BeginDrawTransaction failed: Pool empty. Refunded"));
		return false;
	}

	// 확률 테이블 검증 (없거나 합계가 0이면 뽑기 불가)
	// 이런 상태에서 뽑기를 진행하면 “무조건 1성만” 같은 버그가 나올 수 있음
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
			AddCurrency(Banner.CostCurrencyTag, NeedCost); // 환불
			LR_WARN(TEXT("BeginDrawTransaction failed: RarityRate missing/invalid. Refunded"));
			return false;
		}
	}


	// 천장 계산용: 작업용 pity를 만들어서 “결과 확정 시점”의 NewPity를 산출
	const int32 PrevPity = GetPityCount(BannerID);
	int32 WorkingPity = PrevPity;

	// 결과 롤링(지급X)
	if (!RollResults_NoApply(Banner, Pool, DrawCount, WorkingPity, OutResults))
	{
		AddCurrency(Banner.CostCurrencyTag, NeedCost);	//	환불
		LR_WARN(TEXT("BeginDrawTransaction failed: roll failed. Refunded"));
		return false;
	}

	// 천장 카운터를 "결과 확정 시점"에 반영해두고
	// 취소 시에는 PrevPity로 원복
	if (Banner.bUsePity)
	{
		GachaSave->PityCounterMap.FindOrAdd(BannerID) = WorkingPity;
		OnPityChanged.Broadcast(BannerID, WorkingPity);
	}

	// Pending 트랜잭션 저장(핵심!)
	// 앱이 튕겨도 SaveGame에 남아있어서 복구 가능
	FLRGachaPendingTransaction Pending;
	Pending.TxnId = FGuid::NewGuid();
	Pending.BannerID = BannerID;
	Pending.DrawCount = DrawCount;
	Pending.CostCurrencyTag = Banner.CostCurrencyTag;
	Pending.CostAmount = NeedCost;
	Pending.PrevPity = PrevPity;
	Pending.NewPity = WorkingPity;
	Pending.Results = OutResults;
	Pending.State = ELRGachaTxnState::PendingReveal;

	GachaSave->PendingTransactions.Add(Pending.TxnId, Pending);

	Save();	//	여기서 "비용 차감 + 결과 확정" 저장

	// 락 ON + 상태 브로드캐스트
	bTxnInProgress = true;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::PendingReveal);

	// UI 연출 시작 이벤트(리빌 위젯이 구독해도 되고, Shop가 직접 열어도 됨)
	OnGachaFinished.Broadcast(BannerID, OutResults);

	OutTxnId = Pending.TxnId;

	// 디버그 출력(색깔 로그/온스크린)
	DebugPrintResults(BannerID, DrawCount, OutTxnId, OutResults);

	LR_INFO(TEXT("BeginDrawTransaction OK: %s Txn=%s Cost=%d PrevPity=%d NewPity=%d"),
		*BannerID.ToString(), *OutTxnId.ToString(), NeedCost, PrevPity, WorkingPity);

	return true;

}

// 트랜잭션 커밋 : 실제 지급(컬렉션 반영/중복 골드 지급) + Pending 제거
bool ULRGachaSubsystem::CommitTransaction(const FGuid& TxnId)
{
	if (!GachaSave) return false;

	// Pending 트랜잭션 찾기
	FLRGachaPendingTransaction* PendingPtr = GachaSave->PendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CommitTransaction failed: Pending not found"));
		return false;
	}

	// Remove 전에 필요한 값 복사(안전)
	const FName BannerIDCopy = PendingPtr->BannerID;
	TArray<FLRGachaResult> ResultsCopy = PendingPtr->Results; 

	// 실제 지급(컬렉션에 넣고, 중복이면 골드 지급)
	for (FLRGachaResult& Result : ResultsCopy)
	{
		ApplyResultToCollection(Result);
	}

	// Pending 제거 + 저장
	GachaSave->PendingTransactions.Remove(TxnId);
	Save();

	// 락 해제 + 상태 브로드캐스트
	bTxnInProgress = false;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::Committed);

	LR_INFO(TEXT("CommitTransaction OK: %s Txn=%s"), *BannerIDCopy.ToString(), *TxnId.ToString());
	return true;
}

// 트랜잭션 취소 : 비용 환불 + 천장 원복 + Pending 제거
bool ULRGachaSubsystem::CancelTransaction(const FGuid& TxnId)
{
	if (!GachaSave) return false;

	// Pending 찾기
	FLRGachaPendingTransaction* PendingPtr = GachaSave->PendingTransactions.Find(TxnId);
	if (!PendingPtr)
	{
		LR_WARN(TEXT("CancelTransaction failed: pending not found"));
		return false;
	}

	// Remove 전에 복사(안전)
	const FName BannerIDCopy = PendingPtr->BannerID;
	const FGameplayTag CostTagCopy = PendingPtr->CostCurrencyTag;
	const int32 CostAmountCopy = PendingPtr->CostAmount;
	const int32 PrevPityCopy = PendingPtr->PrevPity;

	// 환불
	AddCurrency(CostTagCopy, CostAmountCopy);

	// 천장 원복
	GachaSave->PityCounterMap.FindOrAdd(BannerIDCopy) = PrevPityCopy;
	OnPityChanged.Broadcast(BannerIDCopy, PrevPityCopy);

	// Pending 제거 + 저장
	GachaSave->PendingTransactions.Remove(TxnId);
	Save();

	// 락 해제 + 상태 브로드캐스트
	bTxnInProgress = false;
	OnGachaTxnStateChanged.Broadcast(ELRGachaTxnState::Canceled);

	LR_INFO(TEXT("CancelTransaction OK: %s Txn=%s Refunded=%d"), *BannerIDCopy.ToString(), *TxnId.ToString(), CostAmountCopy);
	return true;
}

// 결과를 뽑기만 하고 (지급x) Result 배열 구성
bool ULRGachaSubsystem::RollResults_NoApply(const FLRGachaBannerRow& Banner, const TArray<FLRGachaPoolRow>& Pool, 
	int32 DrawCount, int32& InOutPityCounter, TArray<FLRGachaResult>& OutResults)
{
	OutResults.Empty();

	// 컬렉션 시스템(보유 여부 판단용)
	UCollectionSubsystem* CollectionSys = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	if (!CollectionSys)
	{
		LR_WARN(TEXT("RollResults_NoApply: CollectionSubsystem not found"));
		return false;
	}

	TSet<int32> NewlyObtainedInThisTxn;	//	이번 트랜잭션에서 새로 뽑힌 것(중복 판정용)

	for (int32 i = 0; i < DrawCount; i++)
	{
		// 천장 체크(Working Pity 기준)
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

			bPickedOk = PickOneFromPoolUniform(Filtered, Picked);
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

		// "지급 아직 X" 상태에서, UI용으로 New/중복 판정만 미리 계산
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
			// 진짜 신규(보유X, 이번 트랜잭션에서도 첫 등장)
			Result.bIsNew = true;
			NewlyObtainedInThisTxn.Add(Result.ItemID);
		}
		else
		{
			// 중복이면 UI에 "골드 전환량"도 미리 보여줄 수 있게 계산만 해둠
			Result.bIsNew = false;
			Result.bConvertedToGold = true;
			Result.ConvertedGoldAmount = GetDuplicateGold(Result.Rarity);
		}

		// 천장 카운터 갱신(확정된 결과 기준)
		if (Banner.bUsePity)
		{
			if (Result.Rarity == ELRGachaRarity::Legendary)
			{
				InOutPityCounter = 0;	//	전설이면 리셋
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

// 혹시 남아있는 Pending 트랜잭션이 있으면 UI가 복구하도록 조회
bool ULRGachaSubsystem::GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const
{
	OutPending = FLRGachaPendingTransaction();

	if (!GachaSave) return false;
	if (GachaSave->PendingTransactions.Num() <= 0) return false;

	// 하나만 써도 되지만, 일단 첫번째 반환
	for (const auto& Pair : GachaSave->PendingTransactions)
	{
		OutPending = Pair.Value;
		return true;
	}
	return false;
}

// 배너ID 계산 + 트랜잭션 시작
FName ULRGachaSubsystem::GetBannerIdBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType) const
{
	const bool bFullMoon = (TicketType == ELRGachaTicketType::FullMoon);

	if (ItemType == ELRGachaItemType::Hero)
	{
		return bFullMoon ? FName(TEXT("Hero_FullMoon")) : FName(TEXT("Hero_Crescent"));
	}
	else //	Equipment
	{
		return bFullMoon ? FName(TEXT("Equip_FullMoon")) : FName(TEXT("Equip_Crescent"));
	}
}

bool ULRGachaSubsystem::BeginDrawBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType, int32 DrawCount,
	FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults)
{
	const FName BannerID = GetBannerIdBySelection(ItemType, TicketType);

	const bool bOk = BeginDrawTransaction(BannerID, DrawCount, OutTxnId, OutResults);

	// 테스트용 로그: 성공/실패
	if (!bOk)
	{
		LR_WARN(TEXT("[Gacha] BeginDrawBySelection FAILED Banner=%s Count=%d"), *BannerID.ToString(), DrawCount);
	}
	return bOk;

}

// 천장 표시용 함수
int32 ULRGachaSubsystem::GetDisplayPityCount(ELRGachaItemType ItemType) const
{
	const FName FullMoonBannerID =
		(ItemType == ELRGachaItemType::Hero)
		? FName(TEXT("Hero_FullMoon"))
		: FName(TEXT("Equip_FullMoon"));

	return GetPityCount(FullMoonBannerID);
}


// ===================== Debug 옵션 ========================================================
FLinearColor ULRGachaSubsystem::DebugRarityToColor(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return FLinearColor(0.6f, 0.6f, 0.6f, 1.0f); // Gray
	case ELRGachaRarity::Elite:     return FLinearColor(0.20f, 0.55f, 1.00f, 1.0f); // Blue (Elite)
	case ELRGachaRarity::Unique:    return FLinearColor(1.00f, 0.25f, 0.25f, 1.0f); // Red (Unique)
	case ELRGachaRarity::Epic:      return FLinearColor(0.6f, 0.2f, 0.8f, 1.0f); // Purple
	case ELRGachaRarity::Legendary: return FLinearColor(1.0f, 0.8f, 0.2f, 1.0f); // Gold
	default:                        return FLinearColor::White;
	}
}

FString ULRGachaSubsystem::DebugResultToColoredString(const FLRGachaResult& Result) const
{
	const UEnum* ItemTypeEnum = StaticEnum<ELRGachaItemType>();
	const UEnum* RarityEnum = StaticEnum<ELRGachaRarity>();

	const FString ItemTypeStr = ItemTypeEnum ? ItemTypeEnum->GetNameStringByValue((int64)Result.ItemType) : TEXT("UnknownType");
	const FString RarityStr = RarityEnum ? RarityEnum->GetNameStringByValue((int64)Result.Rarity) : TEXT("UnknownRarity");

	// "색 이름"을 문자열에도 같이 붙여서 로그에서 빠르게 보기
	FString ColorName = TEXT("White");
	switch (Result.Rarity)
	{
	case ELRGachaRarity::Common:    ColorName = TEXT("Gray"); break;
	case ELRGachaRarity::Elite:  ColorName = TEXT("Green"); break;
	case ELRGachaRarity::Unique:      ColorName = TEXT("Blue"); break;
	case ELRGachaRarity::Epic:      ColorName = TEXT("Purple"); break;
	case ELRGachaRarity::Legendary: ColorName = TEXT("Gold"); break;
	}

	return FString::Printf(
		TEXT("%s | %s(%s) | ItemID=%d | New=%d | GoldConv=%d(+%d)"),
		*ItemTypeStr,
		*RarityStr, *ColorName,
		Result.ItemID,
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
		LR_INFO(TEXT("[GachaDebug] Banner=%s Count=%d Txn=%s Results=%d"),
			*BannerID.ToString(), DrawCount, *TxnId.ToString(), Results.Num());

		for (int32 i = 0; i < Results.Num(); ++i)
		{
			LR_INFO(TEXT("[GachaDebug] #%02d %s"), i + 1, *DebugResultToColoredString(Results[i]));
		}
	}

	// 화면 (색 표시)
	if (bDebugPrintToScreen && GEngine)
	{
		// 헤더 한 줄
		GEngine->AddOnScreenDebugMessage(
			-1, DebugScreenDuration, FColor::White,
			FString::Printf(TEXT("[GachaDebug] %s  x%d  (%d results)"), *BannerID.ToString(), DrawCount, Results.Num())
		);

		for (int32 i = 0; i < Results.Num(); ++i)
		{
			const FLinearColor LC = DebugRarityToColor(Results[i].Rarity);
			const FColor C = LC.ToFColor(true);

			GEngine->AddOnScreenDebugMessage(
				-1, DebugScreenDuration, C,
				FString::Printf(TEXT("#%02d %s"), i + 1, *DebugResultToColoredString(Results[i]))
			);
		}
	}
}

// 시뮬레이션 전용 =========================================================================================================
static int32 RarityToIndex(ELRGachaRarity R)
{
	switch (R)
	{
	case ELRGachaRarity::Common:    return 1;
	case ELRGachaRarity::Elite:  return 2;
	case ELRGachaRarity::Unique:      return 3;
	case ELRGachaRarity::Epic:      return 4;
	case ELRGachaRarity::Legendary: return 5;
	default: return 0;
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

	// 주의: 실제 저장된 천장 카운트 건드리면 안되니까 로컬 변수로만 시뮬
	int32 SimPity = 0;

	int32 NoLegendaryStreak = 0;

	// 등급 확률 Row 캐시(매번 GetRarityRateRowsForBanner 호출하면 느려질 수 있어서)
	TArray<FLRGachaRarityRateRow> Rates;
	GetRarityRateRowsForBanner(BannerID, Banner.ItemType, Rates);

	// Rates 합계
	auto PickRarity_Stream = [&Stream, &Rates](ELRGachaRarity& OutRarity) -> bool
		{
			float Total = 0.f;
			for (const auto& R : Rates) Total += FMath::Max(0.f, R.Rate);
			if (Total <= 0.f) return false;

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

	// 풀에서 랜덤 1개 선택(Stream 기반)
	auto PickUniform_Stream = [&Stream](const TArray<FLRGachaPoolRow>& In, FLRGachaPoolRow& Out) -> bool
		{
			if (In.Num() <= 0) return false;
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
		if (Star == 1) OutSummary.Count1++;
		else if (Star == 2) OutSummary.Count2++;
		else if (Star == 3) OutSummary.Count3++;
		else if (Star == 4) OutSummary.Count4++;
		else if (Star == 5) OutSummary.Count5++;

		// 천장 카운터 갱신(네 RollResults_NoApply 기준: Legendary면 0, 아니면++)
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
// ============================================================================================================================