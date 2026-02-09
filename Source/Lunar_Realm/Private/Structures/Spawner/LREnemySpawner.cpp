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

// Called when the game starts or when spawned
void ALREnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
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
		PoolSys->InitializePool(EnemyClass, PrewarmCount);
	}

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALREnemySpawner::SpawnEnemy,
		FMath::Max(CurrentSpawnInterval, 0.05f), true);
}

// Called every frame
void ALREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ALREnemySpawner::InitializeFromStageData()
{
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in spawner"));
		return false;
	}

	if (const ULRGameInstance* LRGameInstance = Cast<ULRGameInstance>(GI))
	{
		CurrentStageID = LRGameInstance->GetCurrentStageID();
	}
	// NOTE: StageID 관리 주체 미확정이라 GameInstance의 CurrentStageID를 기본 사용.

	const FStageStaticData& StageData = DataSys->GetStageStaticData(CurrentStageID);
	CachedEnemyIDs = StageData.SpawnEnemyIDs;
	CachedEnemyWeights = StageData.SpawnWeights;
	CurrentSpawnInterval = StageData.SpawnInterval > 0.0f ? StageData.SpawnInterval : DefaultSpawnInterval;
	bIsBossStage = StageData.bIsBossStage;

	if (bSpawnOnlyBossStage != bIsBossStage)
	{
		SetActorTickEnabled(false);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		return false;
	}

	if (CachedEnemyIDs.Num() <= 0)
	{
		LR_WARN(TEXT("Stage(%d) has no enemy IDs"), CurrentStageID);
		return false;
	}

	if (CachedEnemyWeights.Num() != CachedEnemyIDs.Num())
	{
		CachedEnemyWeights.Init(1.0f, CachedEnemyIDs.Num());
	}

	float Sum = 0.0f;
	for (float& Weight : CachedEnemyWeights)
	{
		Weight = FMath::Max(0.0f, Weight);
		Sum += Weight;
	}

	if (Sum <= KINDA_SMALL_NUMBER)
	{
		CachedEnemyWeights.Init(1.0f / CachedEnemyWeights.Num(), CachedEnemyWeights.Num());
	}
	else
	{
		for (float& Weight : CachedEnemyWeights)
		{
			Weight /= Sum;
		}
	}

	return true;
}

int32 ALREnemySpawner::PickEnemyIDByWeight() const
{
	if (CachedEnemyIDs.Num() <= 0)
	{
		return 0;
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
	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys || !EnemyClass)
	{
		return;
	}

	const int32 TargetEnemyID = PickEnemyIDByWeight();
	if (TargetEnemyID <= 0)
	{
		return;
	}

	FTransform SpawnTransform = MakeRandomSpawnTransform();
	ALREnemyCharacter* NewEnemy = PoolSys->Spawn<ALREnemyCharacter>(EnemyClass, SpawnTransform);
	if (!NewEnemy)
	{
		return;
	}

	NewEnemy->InitializeByEnemyID(TargetEnemyID);
}

