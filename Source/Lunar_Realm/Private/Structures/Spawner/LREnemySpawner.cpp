// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Spawner/LREnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Core/LRGameInstance.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "TimerManager.h"

// Sets default values
ALREnemySpawner::ALREnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnAreaBox"));
	SetRootComponent(SpawnAreaBox);
	SpawnAreaBox->SetBoxExtent(FVector(300.0f, 300.0f, 100.0f));
	SpawnAreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// TEMP : 디버그 후 로그 정리 필요
void ALREnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	double StartTime = GetWorld()->GetTimeSeconds();
	LR_INFO(TEXT("=== Spawner BeginPlay START: WorldTime = %.3f ==="), StartTime);

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
		double BeforePoolTime = GetWorld()->GetTimeSeconds();
		LR_INFO(TEXT("Before InitializePool: WorldTime = %.3f"), BeforePoolTime);

		PoolSys->InitializePool(ALREnemyAIController::StaticClass(), PrewarmCount);

		PoolSys->InitializePool(EnemyClass, PrewarmCount);

		double AfterPoolTime = GetWorld()->GetTimeSeconds();
		LR_INFO(TEXT("After InitializePool: WorldTime = %.3f, Elapsed = %.3f"),
			AfterPoolTime, AfterPoolTime - BeforePoolTime);
	}

	double BeforeTimerTime = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALREnemySpawner::SpawnEnemy,
		FMath::Max(CurrentSpawnInterval, 0.05f), true);

	double AfterTimerTime = GetWorld()->GetTimeSeconds();
	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(SpawnTimerHandle);

	LR_INFO(TEXT("Timer Set: WorldTime = %.3f, Interval = %.3f, FirstExecution = %.3f"),
		AfterTimerTime, CurrentSpawnInterval, AfterTimerTime + RemainingTime);
	LR_INFO(TEXT("=== Spawner BeginPlay END: Total Elapsed = %.3f ==="),
		AfterTimerTime - StartTime);
}

// Called every frame
void ALREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// TODO: 리팩토링 필요
bool ALREnemySpawner::InitializeFromStageData()
{
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!GI || !DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in spawner"));
		return false;
	}

	// TODO: 컨벤션 맞춰서 수정 필요
	if (const ULRGameInstance* LRGameInstance = Cast<ULRGameInstance>(GI))
	{
		CurrentStageID = LRGameInstance->GetCurrentStageID();
	}

	// NOTE: StageID -> GameInstance의 CurrentStageID를 임시 사용. => 추후 준범님이 스테이지 매니저 구현하면 거기서 받아올 예정
	if (CurrentStageID == NAME_None)
	{
		LR_WARN(TEXT("EnemySpawner(%s): CurrentStageID is NAME_None. "
			"GameInstance->SetCurrentStageID()가 호출되었는지 확인 필요."), *GetName());
		return false;
	}

	const FStageStaticData& StageData = DataSys->GetStageStaticData(CurrentStageID);

	// TODO: StageManager에서 데이터 가져오는 로직으로 수정
	bool bValidStageData = (StageData.DataID != NAME_None) && (StageData.SpawnEnemyIDs.Num() > 0);
	if (!bValidStageData)
	{
		LR_WARN(TEXT("EnemySpawner(%s): Stage(%s) data invalid or has no SpawnEnemyIDs"),
			*GetName(), *CurrentStageID.ToString());
		return false;
	}
	else
	{
		CachedEnemyIDs = StageData.SpawnEnemyIDs;
		CachedEnemyWeights = StageData.SpawnWeights;
		CurrentSpawnInterval = StageData.SpawnInterval > 0.0f ? StageData.SpawnInterval : DefaultSpawnInterval;
		bIsBossStage = StageData.bIsBossStage;
	}

	if (bSpawnOnlyBossStage != bIsBossStage)
	{
		LR_INFO(TEXT("EnemySpawner(%s): Stage type mismatch (SpawnerBossOnly=%s, StageIsBoss=%s). Skipping."),
			*GetName(),
			bSpawnOnlyBossStage ? TEXT("true") : TEXT("false"),
			bIsBossStage ? TEXT("true") : TEXT("false"));
		return false;
	}

	// 최종 유효성 검사: 캐시된 에너미 ID가 비어있으면 실패
	if (CachedEnemyIDs.Num() <= 0)
	{
		LR_WARN(TEXT("EnemySpawner(%s): Stage(%s) has no enemy IDs"),
			*GetName(), *CurrentStageID.ToString());
		return false;
	}

	//LR_INFO(TEXT("EnemySpawner initialized: Stage(%s), EnemyCount(%d), Interval(%.2f)"),
	//	*CurrentStageID.ToString(), CachedEnemyIDs.Num(), CurrentSpawnInterval);
	
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

void ALREnemySpawner::SpawnEnemy()
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	LR_INFO(TEXT(">>> SpawnEnemy called at WorldTime = %.3f <<<"), CurrentTime);

	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys || !EnemyClass)
	{
		LR_ERROR(TEXT("SpawnEnemy failed: PoolSys or EnemyClass is null"));
		return;
	}

	const FName TargetEnemyID = PickEnemyIDByWeight();
	if (TargetEnemyID == NAME_None)
	{
		LR_WARN(TEXT("Failed to pick EnemyID By Random!"));
		return;
	}

	FTransform SpawnTransform = MakeRandomSpawnTransform();
	LR_INFO(TEXT("SpawnTransform: Location = %s"), *SpawnTransform.GetLocation().ToString());

	ALREnemyCharacter* NewEnemy = PoolSys->Spawn<ALREnemyCharacter>(EnemyClass, SpawnTransform);

	if (!NewEnemy)
	{
		LR_ERROR(TEXT("EnemySpawner(%s): Failed to spawn enemy from pool (returned null)"), *GetName());
		return;
	}

	LR_INFO(TEXT("Enemy spawned successfully: %s at location %s"),
		*NewEnemy->GetName(), *NewEnemy->GetActorLocation().ToString());

	NewEnemy->InitializeByEnemyID(TargetEnemyID);

	LR_INFO(TEXT("Enemy initialized with ID: %s"), *TargetEnemyID.ToString());
}

