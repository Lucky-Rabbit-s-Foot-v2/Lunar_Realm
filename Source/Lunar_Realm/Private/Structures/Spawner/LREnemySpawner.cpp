// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Spawner/LREnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/StageManagerSubsystem.h" // TEST : 실제 빌드 전 해제 
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "Units/Enemy/LREnemyBossCharacter.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "TimerManager.h"
// TEST : 실제 빌드 전 삭제
#include "Core/LRGameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

// Sets default values
ALREnemySpawner::ALREnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnAreaBox"));
	SetRootComponent(SpawnAreaBox);
	SpawnAreaBox->SetBoxExtent(FVector(300.0f, 300.0f, 100.0f));
	SpawnAreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ALREnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	double StartTime = GetWorld()->GetTimeSeconds();

	if (!InitializeFromStageData())
	{
		LR_WARN(TEXT("EnemySpawner(%s) failed to initialize from stage data"), *GetName());
		return;
	}

	if (!EnemyClass)
	{
		LR_WARN(TEXT("EnemySpawner(%s) has no EnemyClass"), *GetName());
		return;
	}

	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (PoolSys)
	{
		PoolSys->InitializePool(ALREnemyAIController::StaticClass(), PrewarmCount);
		PoolSys->InitializePool(EnemyClass, PrewarmCount);
	}

	if (bIsBossStage && CachedBossEnemyID != NAME_None)
	{
		SpawnBoss();
	}

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALREnemySpawner::SpawnEnemy, FMath::Max(CurrentSpawnInterval, 0.05f), true);

	float Rate = FMath::Max(CurrentSpawnInterval, 0.05f);

	if (WaitTime <= 0.0f)
	{
		SpawnEnemy();
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALREnemySpawner::SpawnEnemy, Rate, true);
	}
	else
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALREnemySpawner::SpawnEnemy, Rate, true, WaitTime);
	}
}

// Called every frame
void ALREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/* TEST: 실제 빌드 시 사용 */
//bool ALREnemySpawner::InitializeFromStageData()
//{
//	UGameInstance* GI = GetGameInstance();
//	UStageManagerSubsystem* StageMgr = GI ? GI->GetSubsystem<UStageManagerSubsystem>() : nullptr;
//	if (!GI || !StageMgr)
//	{
//		LR_ERROR(TEXT("StageManagerSubsystem not found in spawner"));
//		return false;
//	}
//
//	CurrentStageID = StageMgr->GetCurrentStageID();
//	if (CurrentStageID == NAME_None)
//	{
//		LR_ERROR(TEXT("EnemySpawner(%s): CurrentStageID is NAME_None. StageManagerSubsystem->LoadStage()가 호출되었는지 확인 필요."), *GetName());
//		return false;
//	}
//
//	bIsBossStage = StageMgr->IsBossStage();
//
//	const FStageSpawnerData& SpawnerData = StageMgr->GetCurrentStageSpawnerData();
//
//	CachedEnemyIDs.Empty();
//	CachedEnemyWeights.Empty();
//	for (const FStageSpawnEnemyData& EnemyEntry : SpawnerData.SpawnableEnemies)
//	{
//		CachedEnemyIDs.Add(EnemyEntry.EnemyID);
//		CachedEnemyWeights.Add(EnemyEntry.SpawnWeight);
//	}
//
//	CurrentSpawnInterval = SpawnerData.SpawnInterval > -0.1f ? SpawnerData.SpawnInterval : DefaultSpawnInterval;
//
//	CachedBossEnemyID = StageMgr->GetBossEnemyID();
//
//	if (CachedEnemyIDs.Num() <= 0 && CachedBossEnemyID == NAME_None)
//	{
//		LR_ERROR(TEXT("EnemySpawner(%s): Stage(%s) has no enemy data and no boss"), *GetName(), *CurrentStageID.ToString());
//		return false;
//	}
//
//	return true;
//}

// TEST : 테스트용 실제 빌드 시 위 함수로 변경
bool ALREnemySpawner::InitializeFromStageData()
{
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!GI || !DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in spawner"));
		return false;
	}

	if (const ULRGameInstance* LRGameInstance = Cast<ULRGameInstance>(GI))
	{
		CurrentStageID = LRGameInstance->GetCurrentStageID();
	}

	if (CurrentStageID == NAME_None)
	{
		LR_WARN(TEXT("EnemySpawner(%s): CurrentStageID is NAME_None. GameInstance->SetCurrentStageID()가 호출되었는지 확인 필요."), *GetName());
		return false;
	}

	const FStageStaticData& StageData = DataSys->GetStageStaticData(CurrentStageID);
	if (StageData.DataID == NAME_None)
	{
		LR_WARN(TEXT("EnemySpawner(%s): Stage(%s) data invalid"), *GetName(), *CurrentStageID.ToString());
		return false;
	}

	bIsBossStage = StageData.bIsBossStage;

	CachedEnemyIDs = StageData.SpawnEnemyIDs;
	CachedEnemyWeights = StageData.SpawnWeights;
	CurrentSpawnInterval = StageData.SpawnInterval > 0.0f ? StageData.SpawnInterval : DefaultSpawnInterval;

	CachedBossEnemyID = StageData.BossEnemyID;

	if (CachedEnemyIDs.Num() <= 0 && CachedBossEnemyID == NAME_None)
	{
		LR_ERROR(TEXT("EnemySpawner(%s): Stage(%s) has no enemy data and no boss"), *GetName(), *CurrentStageID.ToString());
		return false;
	}

	return true;
}

FName ALREnemySpawner::PickEnemyIDByWeight() const
{
	if (CachedEnemyIDs.Num() <= 0)
	{
		return NAME_None;
	}

	const float RandomValue = FMath::FRand();
	float Accumulated = 0.0f;

	for (int32 i = 0; i < CachedEnemyIDs.Num(); ++i)
	{
		const float Weight = CachedEnemyWeights.IsValidIndex(i) ? CachedEnemyWeights[i] : (1.0f / CachedEnemyIDs.Num());
		Accumulated += Weight;
		if (RandomValue <= Accumulated)
		{
			return CachedEnemyIDs[i];
		}
	}

	return CachedEnemyIDs.Last();
}

FTransform ALREnemySpawner::MakeRandomSpawnTransform() const
{
	if (!SpawnAreaBox)
	{
		return GetActorTransform();
	}

	const FVector Origin = SpawnAreaBox->GetComponentLocation();
	const FVector Extent = SpawnAreaBox->GetScaledBoxExtent();
	const FVector RandomLocation = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return FTransform(GetActorRotation(), RandomLocation, FVector::OneVector);
}

void ALREnemySpawner::SpawnBoss()
{
	if (!BossClass)
	{
		LR_WARN(TEXT("EnemySpawner(%s): BossClass is null. Cannot spawn boss."), *GetName());
		return;
	}

	const FTransform SpawnTransform = MakeRandomSpawnTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ALREnemyBossCharacter* BossEnemy = GetWorld()->SpawnActor<ALREnemyBossCharacter>(BossClass, SpawnTransform, SpawnParams);

	if (!BossEnemy)
	{
		LR_ERROR(TEXT("EnemySpawner(%s): Failed to spawn boss actor"), *GetName());
		return;
	}
	
	if (UAbilitySystemComponent* BossASC = BossEnemy->GetAbilitySystemComponent())
	{
		BossASC->AddLooseGameplayTag(LRTags::Team_Enemy_Structure_Core);
	}

	BossEnemy->InitializeByEnemyID(CachedBossEnemyID);
	BossEnemy->InitializeBossSpeed();
	BossEnemy->RegisterMontageNotifyDelegate();

	if (ALREnemyAIController* EnemyAIC = Cast<ALREnemyAIController>(BossEnemy->GetController()))
	{
		float NewRadius = EnemyAIC->GetAttackRange() + DetectionRangeOffset;
		EnemyAIC->SetDetectionRadius(NewRadius);
		BossEnemy->SetCoreAttackOverlapRadius(NewRadius);
	}
}

void ALREnemySpawner::SpawnEnemy()
{
	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys || !EnemyClass)
	{
		LR_ERROR(TEXT("SpawnEnemy failed: PoolSys or EnemyClass is null"));
		return;
	}

	for (int32 i = 0; i < SpawnCountAtOnce; ++i)
	{
		// 1. 스폰할 적 ID 무작위 선택
		const FName TargetEnemyID = PickEnemyIDByWeight();
		if (TargetEnemyID == NAME_None)
		{
			LR_WARN(TEXT("Failed to pick EnemyID By Random!"));
			continue; // 특정 적 스폰에 실패해도 남은 횟수는 계속 진행하도록 continue 사용
		}

		// 2. 무작위 위치 생성 (매 반복마다 새로운 위치 계산)
		FTransform SpawnTransform = MakeRandomSpawnTransform();

		// 3. 풀링 시스템에서 가져오기
		ALREnemyCharacter* NewEnemy = PoolSys->Spawn<ALREnemyCharacter>(EnemyClass, SpawnTransform);

		if (!NewEnemy)
		{
			LR_ERROR(TEXT("EnemySpawner(%s): Failed to spawn enemy from pool (returned null)"), *GetName());
			continue; // 남은 스폰을 위해 continue
		}

		// 4. 데이터 초기화
		NewEnemy->InitializeByEnemyID(TargetEnemyID);
	}
}

