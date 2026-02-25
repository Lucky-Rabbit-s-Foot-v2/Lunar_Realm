// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "Data/LREnumType.h"
#include "Data/LRDataStructs.h"

#include "LRGachaSubsystem.generated.h"

class ULRSaveGame;

/*
 * 이 Subsystem 하나가 가챠 로직 전체를 담당.
 *
 * 책임:
 * - DataTable 로드/캐시 (배너, 풀, 중복보상, 희귀도 확률)
 * - 재화 차감/지급 (통합 CurrencySubsystem 연동)
 * - 천장(피티) 카운트 관리
 * - 뽑기/트랜잭션( Begin/Commit/Cancel ) 처리
 * - 중복 보상(골드 전환) 처리
 * - SaveGame(ULRSaveGame)과 연동하여 Pending 트랜잭션/피티 저장
 * - UI용 델리게이트 브로드캐스트 / 디버그 출력 / 시뮬레이션
 */

 // UI 갱신 델리게이트(재화, 천장, 뽑기 완료)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, ELRCurrencyType, CurrencyType, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPityChanged, FName, BannerID, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGachaFinished, FName, BannerID, const TArray<FLRGachaResult>&, Results);

// 트랜잭션 상태 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGachaTxnStateChanged, ELRGachaTxnState, NewState);

// ───────────────── 시뮬레이션 전용 구조체 ─────────────────

USTRUCT(BlueprintType)
struct FLRGachaSimSummary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 TotalPulls = 0;

	// 등급별 카운트
	UPROPERTY(BlueprintReadOnly)
	int32 Count1 = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Count2 = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Count3 = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Count4 = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Count5 = 0;

	// 천장 관련 통계
	UPROPERTY(BlueprintReadOnly)
	int32 PityTriggered = 0;          // 천장 강제픽 횟수

	UPROPERTY(BlueprintReadOnly)
	int32 MaxNoLegendaryStreak = 0;   // 5성 안 나온 최대 연속 횟수

	// 로그용 문자열(한 줄 요약)
	FString ToString() const;
};

/**
 * 가챠 전용 GameInstanceSubsystem
 *
 * - 재화/천장/뽑기/중복 처리/저장/시뮬레이션을 모두 여기에서 처리.
 * - UI/다른 시스템은 여기의 API만 호출하면 됨.
 */
UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ULRGachaSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// UGameInstanceSubsystem override
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ───────────────── 재화 API ─────────────────

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	int32 GetCurrency(ELRCurrencyType Type) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	void AddCurrency(ELRCurrencyType Type, int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	bool SpendCurrency(ELRCurrencyType Type, int32 Cost);

	// ───────────────── 천장(Pity) API ─────────────────

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Pity")
	int32 GetPityCount(FName BannerID) const;

	// ───────────────── 뽑기 가능 여부 ─────────────────

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool CanDraw(FName BannerID, int32 DrawCount, int32& OutNeedCost) const;

	// ───────────────── 델리게이트 (위젯 바인딩용) ─────────────────

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnCurrencyChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnPityChanged OnPityChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaFinished OnGachaFinished;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaTxnStateChanged OnGachaTxnStateChanged;

	// ───────────────── 트랜잭션 기반 API ─────────────────

	/** 트랜잭션 시작: 비용 차감 + 결과 확정/저장 (지급은 Commit에서만) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	/** 트랜잭션 커밋: 실제 지급(컬렉션 반영/중복 골드 지급) + Pending 제거 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CommitTransaction(const FGuid& TxnId);

	/** 트랜잭션 취소: 비용 환불 + 천장 원복 + Pending 제거 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CancelTransaction(const FGuid& TxnId);

	/** 남아있는 Pending 트랜잭션이 있는지 조회 (튕김 복구용) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const;

	// ───────────────── UI 편의 함수 ─────────────────

	/** 선택(영웅/장비 + 티켓 종류)으로 배너ID 계산 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	FName GetBannerIdBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType) const;

	/** 선택 기반으로 BeginDrawTransaction 호출하는 편의 함수 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool BeginDrawBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType, int32 DrawCount,
		FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	/** 천장 표시용(FullMoon 배너 기준) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Pity")
	int32 GetDisplayPityCount(ELRGachaItemType ItemType) const;

	// ───────────────── 리빌 맵 연동용 Pending Reveal ─────────────────

	/** 로비에서 뽑기 후, 리빌 맵으로 넘길 결과를 캐시에 저장 */
	void SetPendingReveal(FName InBannerID, const FGuid& InTxnId, const TArray<FLRGachaResult>& InResults);

	/** 리빌 맵 진입 시, 캐시에 저장된 결과를 꺼내서 소비 */
	bool ConsumePendingReveal(FName& OutBannerID, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

protected:
	// ───────────────── DataTable Soft Reference (에디터에서 지정) ─────────────────

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> BannerDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> PoolDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> DuplicateRewardDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> RarityRateDataTable;

private:
	// ───────────────── 로드된 DataTable 캐시 ─────────────────

	UPROPERTY()
	UDataTable* LoadedBannerDT = nullptr;

	UPROPERTY()
	UDataTable* LoadedPoolDT = nullptr;

	UPROPERTY()
	UDataTable* LoadedDupRewardDT = nullptr;

	UPROPERTY()
	UDataTable* LoadedRarityRateDT = nullptr;

	/** SaveGame Slot UserIndex (현재 단일 사용자 가정) */
	const uint32 UserIndex = 0;

	/** 트랜잭션 락 (버튼 연타 방지용) */
	bool bTxnInProgress = false;

	/** 결과만 뽑고(지급X) FLRGachaResult 배열 구성 */
	bool RollResults_NoApply(const FLRGachaBannerRow& Banner, const TArray<FLRGachaPoolRow>& Pool,
		int32 DrawCount, int32& InOutPityCounter, TArray<FLRGachaResult>& OutResults);

	// ───────────────── 내부 로직 헬퍼 ─────────────────

	void LoadDataTables();

	bool GetBannerRow(FName BannerID, FLRGachaBannerRow& OutRow) const;

	/** 배너에 해당하는 풀 목록 가져오기 */
	void GetPoolRowsForBanner(FName BannerID, TArray<FLRGachaPoolRow>& OutRows) const;

	/** 배너/아이템 타입에 맞는 등급 확률 Row들 가져오기 */
	void GetRarityRateRowsForBanner(FName BannerID, ELRGachaItemType ItemType, TArray<FLRGachaRarityRateRow>& OutRows) const;

	/** 등급 확률로 등급 1개 뽑기 */
	bool PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const;

	/** 풀에서 균등 랜덤으로 1개 선택 (등급 내부 균등) */
	bool PickOneFromPoolUniform(const TArray<FLRGachaPoolRow>& Pool, FLRGachaPoolRow& OutPicked) const;

	/** 특정 등급만 강제해서 뽑기 (천장용) */
	bool PickOneFromPoolByRarity(const TArray<FLRGachaPoolRow>& Pool, ELRGachaRarity TargetRarity, FLRGachaPoolRow& OutPicked) const;

	/** 중복 보상으로 지급할 골드량 조회 */
	int32 GetDuplicateGold(ELRGachaRarity Rarity) const;

	/** 결과를 컬렉션에 반영, 신규 여부 반환 */
	bool TryAddToCollection(const FLRGachaResult& Result, bool& bOutWasNew);

	/** 천장 카운트 +1 */
	void IncrementPity(FName BannerID);

	/** 천장 카운트 리셋 */
	void ResetPity(FName BannerID);

	// ───────────────── 서브시스템 참조 ─────────────────

	UPROPERTY()
	UCurrencySubsystem* CurrencySubsystem = nullptr;

	UPROPERTY()
	USaveGameSubsystem* SaveGameSubsystem = nullptr; // Pending/Pity 저장용

	// SaveGame 헬퍼
	ULRSaveGame* GetSaveGame() const;

	// ───────────────── Pending Reveal 상태 ─────────────────

	bool bHasPendingReveal = false;

	/** 마지막 뽑기에서 리빌해야 할 배너 */
	FName PendingBannerID;

	/** 마지막 뽑기 트랜잭션 ID */
	FGuid PendingTxnId;

	/** 리빌 맵으로 넘길 결과 배열 */
	TArray<FLRGachaResult> PendingResults;

public:
	// ───────────────── Debug 옵션 ─────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToLog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	float DebugScreenDuration = 6.0f;

	/** 결과 1개를 "색 텍스트"로 (로그/화면 공용) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FString DebugResultToColoredString(const FLRGachaResult& Result) const;

	/** 등급 -> 화면 색 매핑 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FLinearColor DebugRarityToColor(ELRGachaRarity Rarity) const;

	/** 결과 배열을 한 번에 출력(로그/화면) */
	void DebugPrintResults(FName BannerID, int32 DrawCount, const FGuid& TxnId, const TArray<FLRGachaResult>& Results) const;

	/** 시뮬레이션 전용 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	bool Debug_SimulateBanner(
		FName BannerID,
		int32 TotalPulls,
		int32 Seed,
		FLRGachaSimSummary& OutSummary,
		bool bOverrideUsePity = false,
		bool bUsePityOverrideValue = false
	);
};