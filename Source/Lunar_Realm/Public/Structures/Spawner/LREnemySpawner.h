// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/PoolingSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "LREnemySpawner.generated.h"

class ALREnemyCharacter;

/**
 * 에너미 스포너 클래스
 * - GameDataSubsystem 에서 받아온 키값들 멤버로 저장
 * - 스테이지 정보 받아와서 해당하는 Enemy 후보 선택
 * - 스폰은 오브젝트 풀링 시스템 사용
 */
 //============================================================================
 // (260205) KWB 제작. 제반 사항 구현.
 // TODO: 파싱 시스템 구현(enum 기반으로 변경 - TBD) / 오브젝트 풀링 파트 구현 필요
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

	UFUNCTION()
	void SpawnEnemy();

	UPROPERTY()
	TSubclassOf<ALREnemyCharacter> TargetEnemeyclass = nullptr;

	UPROPERTY()
	TArray<int32> CachedEnemyIDs;

	UPROPERTY()
	int32 TargetEnemyID;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USphereComponent> SpawnRadiusVisualizer;

	FTimerHandle SpawnTimerHandle;

private:
	UPROPERTY(VisibleAnywhere, Category = "LR|Components")
	class UArrowComponent* ArrowComp = nullptr;
};
