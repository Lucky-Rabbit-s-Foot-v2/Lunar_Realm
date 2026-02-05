// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/Gacha/LRGachaTransactionTypes.h"
#include "Data/Gacha/LRGachaTypes.h"

#include "LRGachaSubsystem.generated.h"

class ULRGachaSaveGame;

/*
* 이 Subsystem 하나가 가챠의 로직을 모두 담당.
* UI는 여기 API만 호출하고, 결과를 받아서 연출/표시만 한다.
*/

// UI갱신 델리게이트(재화/뽑기완료)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, FGameplayTag, CurrencyTag, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPityChanged, FName, BannerID, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGachaFinished, FName, BannerID, const TArray<FLRGachaResult>&, Results);

// 트랜잭션 상태 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGachaTxnStateChanged, ELRGachaTxnState, NewState);

// 시뮬레이션 전용===================================================================================================
USTRUCT(BlueprintType)
struct FLRGachaSimSummary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 TotalPulls = 0;

	// 등급별 카운트
	UPROPERTY(BlueprintReadOnly) int32 Count1 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count2 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count3 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count4 = 0;
	UPROPERTY(BlueprintReadOnly) int32 Count5 = 0;

	// 천장 관련 통계
	UPROPERTY(BlueprintReadOnly) int32 PityTriggered = 0;         // (InOutPity+1>=Threshold)로 "천장 강제픽" 들어간 횟수
	UPROPERTY(BlueprintReadOnly) int32 MaxNoLegendaryStreak = 0;  // 5성 안 나온 최대 연속 횟수

	// 로그용 문자열(한 줄로 보기)
	FString ToString() const;
};
// =========================================================================================================================

/**
 * 이 Subsystem 하나가 재화/천장/뽑기/중복처리/저장을 모두 담당하므로 여기만 호출하면됨
 */
UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ULRGachaSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Currency(재화) API ==========================================
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	int32 GetCurrency(FGameplayTag CurrencyTag) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	void AddCurrency(FGameplayTag CurrencyTag, int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Currency")
	bool SpendCurrency(FGameplayTag CurrencyTag, int32 Cost);

	// Pity(천장) API ================================================
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Pity")
	int32 GetPityCount(FName BannerID) const;

	// Draw API(뽑기) ========================================================================================
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool CanDraw(FName BannerID, int32 DrawCount, int32& OutNeedCost) const;

	// 실제 뽑기 실행(1회/10회)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool Draw(FName BannerID, int32 DrawCount, TArray<FLRGachaResult>& OutResults);

	// 델리게이트(위젯 바인딩) =================================================================================
	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnCurrencyChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnPityChanged OnPityChanged;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaFinished OnGachaFinished;
	// =======================================================================================================
	// 트랜잭션 기반 API ======================================================================================
	
	// 트랜잭션 시작 : 비용 차감 + 결과 확정/저장 + (지급은 아직!)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool BeginDrawTransaction(FName BannerID, int32 DrawCount, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	// 트랜잭션 커밋 : 실제 지급(컬렉션 반영/중복 골드 지급) + Pending 제거
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CommitTransaction(const FGuid& TxnId);

	// 트랜잭션 취소 : 비용 환불 + 천장 원복 + Pending 제거
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool CancelTransaction(const FGuid& TxnId);

	// 혹시 남아있는 Pending 트랜잭션이 있으면 UI가 복구하도록 조회
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Txn")
	bool GetAnyPendingTransaction(FLRGachaPendingTransaction& OutPending) const;

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Event")
	FOnGachaTxnStateChanged OnGachaTxnStateChanged;
	 
	// UI 편의 함수 =======================================================================================
	// 배너ID 계산 + 트랜잭션 시작
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	FName GetBannerIdBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Draw")
	bool BeginDrawBySelection(ELRGachaItemType ItemType, ELRGachaTicketType TicketType, int32 DrawCount,
		FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);

	// 천장 표시용 함수
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Pity")
	int32 GetDisplayPityCount(ELRGachaItemType ItemType) const;

protected:
	// DataTable Soft Reference ==============================
	// DT만 꽂으면 되게 단순화
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> BannerDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> PoolDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> DuplicateRewardDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Data")
	TSoftObjectPtr<UDataTable> RarityRateDataTable;

private:
	// 로드된 DT 캐시 ========================================
	UPROPERTY()
	UDataTable* LoadedBannerDT = nullptr;

	UPROPERTY()
	UDataTable* LoadedPoolDT = nullptr;

	UPROPERTY()
	UDataTable* LoadedDupRewardDT = nullptr;
		
	UPROPERTY()
	UDataTable* LoadedRarityRateDT = nullptr;

	// 저장 데이터 ===========================================
	UPROPERTY()
	TObjectPtr<ULRGachaSaveGame> GachaSave;

	// 슬롯 이름(기존 PlayerSaveSlot과 분리해서 충돌 방지)
	const FString SaveSlotName = TEXT("GachaSaveSlot");
	const uint32 UserIndex = 0;

	// 트랜잭션 락(연타 방지)
	bool bTxnInProgress = false;

	// 결과를 뽑기만 하고 (지급x) Result 배열 구성
	bool RollResults_NoApply(const FLRGachaBannerRow& Banner, const TArray<FLRGachaPoolRow>& Pool, int32 DrawCount,
		int32& InOutPityCounter, TArray<FLRGachaResult>& OutResults);
private:
	// 내부 로직 ==================================
	void LoadOrCreateSave();
	void Save();

	void LoadDataTables();

	bool GetBannerRow(FName BannerID, FLRGachaBannerRow& OutRow) const;

	// 배너 풀 목록 가져오기
	void GetPoolRowsForBanner(FName BannerID, TArray<FLRGachaPoolRow>& OutRows) const;

	// 배너/타입에 맞는 등급 확률 Row들 가져오기
	void GetRarityRateRowsForBanner(FName BannerID, ELRGachaItemType ItemType, TArray<FLRGachaRarityRateRow>& OutRows) const;

	// 등급 확률로 등급 1개 뽑기
	bool PickRarityByRates(FName BannerID, ELRGachaItemType ItemType, ELRGachaRarity& OutRarity) const;

	// Pool에서 균등 랜덤으로 1개 선택(등급 내부는 균등)
	bool PickOneFromPoolUniform(const TArray<FLRGachaPoolRow>& Pool, FLRGachaPoolRow& OutPicked) const;

	// 특정 등급만 강제해서 뽑기(천장용)
	bool PickOneFromPoolByRarity(const TArray<FLRGachaPoolRow>& Pool, ELRGachaRarity TargetRarity, FLRGachaPoolRow& OutPicked) const;

	// 중복보상 골드량 조회
	int32 GetDuplicateGold(ELRGachaRarity Rarity) const;

	// 실제 획득 처리(컬렉션에 추가 or 중복이면 골드)
	void ApplyResultToCollection(FLRGachaResult& InOutResult);

	// 천장 카운트 조작
	void IncrementPity(FName BannerID);
	void ResetPity(FName BannerID);

	// 태그 헬퍼(골드)
	FGameplayTag GetGoldTag() const;


public:
	// ===================== Debug 옵션 =====================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToLog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	bool bDebugPrintToScreen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Debug")
	float DebugScreenDuration = 6.0f;

	// 결과 1개를 "색 텍스트"로 (로그/화면 공용)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FString DebugResultToColoredString(const FLRGachaResult& Result) const;

	// 등급 -> 화면 색
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Debug")
	FLinearColor DebugRarityToColor(ELRGachaRarity Rarity) const;

	// 결과 배열을 한번에 출력(로그/화면)
	void DebugPrintResults(FName BannerID, int32 DrawCount, const FGuid& TxnId, const TArray<FLRGachaResult>& Results) const;

public:
	//시뮬레이션 전용
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
