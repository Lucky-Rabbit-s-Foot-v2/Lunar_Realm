// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/LRDataStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StageManagerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FStageSpawnEnemyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	FName EnemyID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	float SpawnWeight;
};

USTRUCT(BlueprintType)
struct FStageSpawnerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TArray<FStageSpawnEnemyData> SpawnableEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	float SpawnInterval = 1.f;
	
	void SetByStaticData(const FStageStaticData& InData);
};

USTRUCT(BlueprintType)
struct FStageRewardData
{
	GENERATED_BODY()

	void SetByStaticData(const FStageStaticData& InData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 Gold = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 NormalTicket = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 EnhanceTicket = 0;

};


// =============================================================================
/**
 * 스테이지 정보 관리 서브시스템
 * - 스테이지 관련 제반 사항 관리
 * - 스테이지 클리어, 실패 등 이벤트 관리
 * - 스테이지 진행 상황 관리
 */
 //=============================================================================
 // (260211) PJB 제작. 제반 사항 구현.
 // (260318) KWB GetBossEnemyID() 함수 추가
 //=============================================================================

UCLASS()
class LUNAR_REALM_API UStageManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 스테이지 맵 이동 전에 호출할 것. 정적 데이터 로드 및 캐싱 수행.
	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	void LoadStage(FName StageID);

	// ========================================
	// 현재 스테이지 데이터 접근
	// ========================================
	
	// 블루 프린트 테스트용 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "LR|Test")
	FStageStaticData GetCurrentStageDataCopy() const;

	const FStageStaticData* GetCurrentStateData();

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	const FStageSpawnerData& GetCurrentStageSpawnerData();
	
	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	const FStageRewardData& GetCurrentStageRewardData();

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	const FString GetStageName() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	bool IsBossStage() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	FName GetBossEnemyID() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	FName GetCurrentStageID() const;

	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	FStageClearedData GetStageClearedData(FName StageID);

private:
	void CacheCurrentStageData();

	const FStageStaticData* CurrentStageData = nullptr;

	UPROPERTY()
	FStageSpawnerData CachedSpawnerData;
	
	UPROPERTY()
	FStageRewardData CachedRewardData;
};
