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
 * - GameDataSubsystem 에서 받아온 키값들 멤버로 저장
 * - 스테이지 정보 받아와서 해당하는 Enemy 후보 선택
 * - 스폰은 오브젝트 풀링 시스템 사용
 */
 //============================================================================
 // (260205) KWB 제작. 제반 사항 구현.
 // (260208) Stage 데이터 드리븐, 가중치 설정, 보스 스테이지 필터링.
 // (260210) KWB 키값 타입 int32 -> FName 으로 변경 반영
 //============================================================================
UCLASS()
class LUNAR_REALM_API ALREnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALREnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

protected:
	UFUNCTION(BlueprintCallable)
	bool InitializeFromStageData();

	FName PickEnemyIDByWeight() const;
	FTransform MakeRandomSpawnTransform() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TSubclassOf<ALREnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	bool bSpawnOnlyBossStage = false;

	// 사전 생성 오브젝트 풀 - 추후 개수 변경 필요 (50 ~ 100)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	int32 PrewarmCount = 5;	// TEMP

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	float DefaultSpawnInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TObjectPtr<UBoxComponent> SpawnAreaBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TArray<FName> CachedEnemyIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	TArray<float> CachedEnemyWeights;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	FName CurrentStageID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	bool bIsBossStage = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Spawner")
	float CurrentSpawnInterval = 1.0f;

	FTimerHandle SpawnTimerHandle;
};
