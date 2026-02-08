// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/LRDataStructs.h"
#include "LREnemySpawner.generated.h"

class ALREnemyCharacter;
class UBoxComponent;

/**
 * 에너미 스포너 클래스
 * - StageStaticData 기반 ID/가중치 확정 후 타이머 스폰
 * - 스폰은 오브젝트 풀링 시스템 사용
 */
//============================================================================
// (260205) KWB 제작.
// (260208) Codex 확장. Stage 데이터 드리븐 + 가중치 랜덤 + 보스 스테이지 필터링.
//============================================================================
UCLASS()
class LUNAR_REALM_API ALREnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:
	ALREnemySpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

protected:
	UFUNCTION(BlueprintCallable)
	bool InitializeFromStageData();

	int32 PickEnemyIDByWeight() const;
	FTransform MakeRandomSpawnTransform() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TSubclassOf<ALREnemyCharacter> EnemyClass;

	// true면 보스 스테이지만 동작, false면 일반 스테이지만 동작
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	bool bSpawnOnlyBossStage = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	int32 PrewarmCount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	float DefaultSpawnInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TObjectPtr<UBoxComponent> SpawnAreaBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TArray<int32> CachedEnemyIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TArray<float> CachedEnemyWeights;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	int32 CurrentStageID = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	bool bIsBossStage = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	float CurrentSpawnInterval = 1.0f;

	FTimerHandle SpawnTimerHandle;
};
