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
	if (!GI || !DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in spawner"));
		return false;
	}

	if (const ULRGameInstance* LRGameInstance = Cast<ULRGameInstance>(GI))
	{
		CurrentStageID = LRGameInstance->GetCurrentStageID();
	}
	// NOTE: StageID -> GameInstance의 CurrentStageID를 임시 사용. => 추후 준범님이 스테이지 매니저 구현하면 거기서 받아올 예정

	const FStageStaticData& StageData = DataSys->GetStageStaticData(CurrentStageID);

	// 스테이지 데이터 검사[1 스테이지 데이터로 폴백, 1 스테이지 데이터 없을 시 하드 코딩된 데이터 사용]
	// TODO: StageManager에서 데이터 가져오는 로직으로 수정

	bool bValidStageData = (StageData.DataID != NAME_None) && (StageData.SpawnEnemyIDs.Num() > 0);
	if (!bValidStageData)
	{
		LR_WARN(TEXT("Invalid Stage Data"));
		return false;

	//	// 현재 스테이지 데이터를 불러올 수 없으면 1스테이지로 폴백
	//	LR_WARN(TEXT("Stage(%d) data not found or has no enemy IDs. Falling back to Stage 1."), CurrentStageID);
	//	CurrentStageID = 1;
	//	const FStageStaticData& FallbackData = DataSys->GetStageStaticData(1);
	//	bValidStageData = (FallbackData.StageID != 0) && (FallbackData.SpawnEnemyIDs.Num() > 0);	// FIX(KWB) : FName으로 수정 필요한 부분
	//	if (bValidStageData)
	//	{
	//		CachedEnemyIDs = FallbackData.SpawnEnemyIDs;
	//		CachedEnemyWeights = FallbackData.SpawnWeights;
	//		CurrentSpawnInterval = FallbackData.SpawnInterval > 0.0f ? FallbackData.SpawnInterval : DefaultSpawnInterval;
	//		bIsBossStage = FallbackData.bIsBossStage;
	//	}
	//	else
	//	{
	//		// 1스테이지 데이터도 없으면 하드코딩 기본값으로 세팅
	//		LR_WARN(TEXT("Stage 1 data also not found. Using hardcoded defaults for spawner."));
	//		CachedEnemyIDs = { 31010101 };
	//		CachedEnemyWeights = { 1.0f };
	//		CurrentSpawnInterval = DefaultSpawnInterval;
	//		bIsBossStage = false;
	//	}
	//}
	//else if (!bValidStageData)
	//{
	//	// CurrentStageID == 1인데도 데이터가 없는 경우
	//	LR_WARN(TEXT("Stage 1 data not found in DataTable. Using hardcoded defaults for spawner."));
	//	CachedEnemyIDs = { 31010101 };
	//	CachedEnemyWeights = { 1.0f };
	//	CurrentSpawnInterval = DefaultSpawnInterval;
	//	bIsBossStage = false;
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
		if (CachedEnemyIDs.Num() <= 0)
		{
			LR_WARN(TEXT("Stage(%s) has no enemy IDs after fallback"), *CurrentStageID.ToString());
			return false;
		}
	}

	LR_INFO(TEXT("EnemySpawner initialized: Stage(%s), EnemyCount(%d), Interval(%.2f)"),
		*CurrentStageID.ToString(), CachedEnemyIDs.Num(), CurrentSpawnInterval);
	
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
			return CachedEnemyIDs[i]; // FIX
		}
	}

	return CachedEnemyIDs.Last();	// FIX
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

	const FName TargetEnemyID = PickEnemyIDByWeight();
	if (TargetEnemyID == NAME_None)
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

