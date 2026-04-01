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

/**
 * ULRGachaSubsystem (가챠 전담 GameInstanceSubsystem)
 *
 * 역할
 * - DataTable 로드/캐시: 배너 / 풀 / 중복 보상 / 등급 확률 / 리빌 비주얼
 * - 재화 차감/지급: CurrencySubsystem 연동
 * - 천장(피티) 카운트 저장/갱신: SaveGame 연동
 * - 트랜잭션 기반 뽑기: Begin / Commit / Cancel
 * - 중복 처리: 골드 전환
 * - UI 편의: 델리게이트 브로드캐스트 + PendingReveal 캐시
 * - 리빌 화면용 표시 데이터 구성
 *
 * 원칙
 * - 결과 확정/저장/지급은 이 Subsystem에서만 처리
 * - UI/Actor는 이 Subsystem의 API를 통해서만 데이터를 받도록 유지
 */

 // ───────────────── UI 갱신 델리게이트 ─────────────────
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, ELRCurrencyType, CurrencyType, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPityChanged, FName, BannerID, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGachaFinished, FName, BannerID, const TArray<FLRGachaResult>&, Results);

// ───────────────── 트랜잭션 상태 델리게이트 ─────────────────
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGachaTxnStateChanged, ELRGachaTxnState, NewState);

// ───────────────── 시뮬레이션 결과 구조체 ─────────────────
USTRUCT(BlueprintType)
struct FLRGachaSimSummary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 TotalPulls = 0;

	UPROPERTY(BlueprintReadOnly) int32 Count1 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count2 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count3 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count4 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count5 = 0;

	UPROPERTY(BlueprintReadOnly) int32 PityTriggered = 0;
	UPROPERTY(BlueprintReadOnly) int32 MaxNoLegendaryStreak = 0;

	/** 로그 출력용 한 줄 요약 문자열 */
	FString ToString() const;
};

UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ULRGachaSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ───────────────── Flow(로비 복귀 처리) ─────────────────

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	void SetLastShopBanner(FName InBannerID);

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	FName GetLastShopBanner() const;

	/** 로비 복귀 시 샵 자동 오픈 플래그 끄기(Home 같은 케이스) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	void ClearOpenShopOnLobbyReturn();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** 리빌 종료 후 로비로 복귀할 때 샵 자동 오픈 요청 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	void RequestOpenShopOnLobbyReturn(FName InBannerID);

	/** Shop이 열릴 때 배너 복구용(1회 소비) */
	bool ConsumePendingReturnShopBanner(FName& OutBannerID);

	/** 소비하지 않고 보기만 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	bool PeekPendingReturnShopBanner(FName& OutBannerID) const;

	/** 수동 초기화 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Flow")
	void ClearPendingReturnShopBanner();

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

	// ───────────────── 델리게이트(UI 바인딩용) ─────────────────

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnCurrencyChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnPityChanged OnPityChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaFinished OnGachaFinished;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaTxnStateChanged OnGachaTxnStateChanged;

	// ───────────────── 트랜잭션 기반 API ─────────────────

	/** 트랜잭션 시작: 비용 차감 + 결과 확정/저장(지급은 Commit에서만) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	/** 트랜잭션 커밋: 실제 지급(컬렉션 반영/중복 골드 지급) + Pending 제거 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CommitTransaction(const FGuid& TxnId);

	/** 트랜잭션 취소: 비용 환불 + 천장 원복 + Pending 제거 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CancelTransaction(const FGuid& TxnId);

	/** Pending 트랜잭션 조회(튕김 복구용) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const;

	// ───────────────── UI 편의 함수 ─────────────────

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	FName GetBannerIdBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool BeginDrawBySelection(
		ELRGachaItemType ItemType,
		ELRGachaTicketType TicketType,
		int32 DrawCount,
		FGuid& OutTxnId,
		TArray<FLRGachaResult>& OutResults
	);

	/** FullMoon 기준 천장 표시용 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Pity")
	int32 GetDisplayPityCount(ELRGachaItemType ItemType) const;

	bool GetRarityRatesForBanner(
		FName BannerID,
		ELRGachaItemType ItemType,
		TMap<ELRGachaRarity, float>& OutRates
	) const;

	// ───────────────── 리빌 맵 연동: Pending Reveal ─────────────────

	void SetPendingReveal(FName InBannerID, const FGuid& InTxnId, const TArray<FLRGachaResult>& InResults);
	bool ConsumePendingReveal(FName& OutBannerID, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	// ───────────────── 리빌 화면 표시 데이터 구성 ─────────────────

	/**
	 * 결과 1개를 리빌 화면 표시용 데이터로 구성
	 * - 리빌 전용 DT 우선
	 * - 비어 있으면 Character/Equipment StaticData의 기본 이미지 fallback
	 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	bool BuildRevealPresentationData(const FLRGachaResult& Result, FLRGachaRevealPresentationData& OutData) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	UTexture2D* GetResultSlotTexture(FName ItemID, ELRGachaItemType ItemType) const;

protected:
	// ───────────────── DataTable Soft Reference(에디터 지정 가능) ─────────────────

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> BannerDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> PoolDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> DuplicateRewardDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> RarityRateDataTable;

	/** 가챠 리빌 연출 전용 비주얼 DT */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> RevealVisualDataTable;

private:
	// 샵에서 마지막으로 선택된 배너(탭 복구용)
	FName LastShopBannerID = NAME_None;

	// ───────────────── 맵 로드 후 훅(로비 복귀 자동 UI 처리) ─────────────────

	void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);

	// 로비 복귀 후 Shop 자동 오픈 상태
	bool bOpenShopOnLobbyReturn = false;
	FName PendingReturnShopBannerID = NAME_None;

	// ───────────────── 로드된 DataTable 캐시 ─────────────────

	UPROPERTY() UDataTable* LoadedBannerDT = nullptr;
	UPROPERTY() UDataTable* LoadedPoolDT = nullptr;
	UPROPERTY() UDataTable* LoadedDupRewardDT = nullptr;
	UPROPERTY() UDataTable* LoadedRarityRateDT = nullptr;
	UPROPERTY() UDataTable* LoadedRevealVisualDT = nullptr;

	// SaveGame slot index(단일 사용자 가정)
	const uint32 UserIndex = 0;

	// 트랜잭션 락(연타/중복 실행 방지)
	bool bTxnInProgress = false;

	// 결과만 뽑고(지급X) FLRGachaResult 배열 구성
	bool RollResults_NoApply(
		const FLRGachaBannerRow& Banner,
		const TArray<FLRGachaPoolRow>& Pool,
		int32 DrawCount,
		int32& InOutPityCounter,
		TArray<FLRGachaResult>& OutResults
	);

	// ───────────────── 내부 헬퍼(데이터 조회/선택) ─────────────────

	void LoadDataTables();

	bool GetBannerRow(FName BannerID, FLRGachaBannerRow& OutRow) const;
	void GetPoolRowsForBanner(FName BannerID, TArray<FLRGachaPoolRow>& OutRows) const;
	void GetRarityRateRowsForBanner(FName BannerID, ELRGachaItemType ItemType, TArray<FLRGachaRarityRateRow>& OutRows) const;

	/** 리빌 비주얼 DT에서 ItemID 기준 Row 찾기 */
	bool GetRevealVisualRow(FName ItemID, ELRGachaItemType ItemType, FLRGachaRevealVisualRow& OutRow) const;

	bool PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const;
	bool PickOneFromPoolUniform(const TArray<FLRGachaPoolRow>& Pool, FLRGachaPoolRow& OutPicked) const;
	bool PickOneFromPoolByRarity(const TArray<FLRGachaPoolRow>& Pool, ELRGachaRarity TargetRarity, FLRGachaPoolRow& OutPicked) const;

	// 중복 보상(골드 전환)
	int32 GetDuplicateGold(ELRGachaRarity Rarity) const;

	// 컬렉션 반영(신규 여부 반환)
	bool TryAddToCollection(const FLRGachaResult& Result, bool& bOutWasNew);

	// 천장 카운트 조작
	void IncrementPity(FName BannerID);
	void ResetPity(FName BannerID);

	// ───────────────── 서브시스템 참조 ─────────────────

	UPROPERTY() UCurrencySubsystem* CurrencySubsystem = nullptr;
	UPROPERTY() USaveGameSubsystem* SaveGameSubsystem = nullptr;

	ULRSaveGame* GetSaveGame() const;

	// ───────────────── Pending Reveal 상태 ─────────────────

	bool bHasPendingReveal = false;
	FName PendingBannerID;
	FGuid PendingTxnId;
	TArray<FLRGachaResult> PendingResults;

public:
	// ───────────────── Debug 옵션 ─────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToLog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	float DebugScreenDuration = 6.0f;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FString DebugResultToColoredString(const FLRGachaResult& Result) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FLinearColor DebugRarityToColor(ELRGachaRarity Rarity) const;

	void DebugPrintResults(FName BannerID, int32 DrawCount, const FGuid& TxnId, const TArray<FLRGachaResult>& Results) const;

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