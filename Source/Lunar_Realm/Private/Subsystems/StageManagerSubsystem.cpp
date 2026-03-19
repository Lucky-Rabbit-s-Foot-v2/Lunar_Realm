// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/StageManagerSubsystem.h"

#include "Engine/GameInstance.h"
#include "System/LoggingSystem.h"
#include "Data/LRGameDataConfig.h"
#include "Subsystems/GameDataSubsystem.h"

void UStageManagerSubsystem::LoadStage(FName StageID)
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	if (!ensure(GameDataSubsystem))
	{
		LR_SCREEN_INFO(TEXT("GameDataSubsystem is null"));
		return;
	}

	const FStageStaticData& RefData = GameDataSubsystem->GetStageStaticData(StageID);
	
	if (RefData.DataID == NAME_None)
	{
		LR_INFO(TEXT("Failed to load stage. Invalid StageID: %s"), *StageID.ToString());
		CurrentStageData = nullptr;
		return;
	}

	CurrentStageData = &RefData;
	CacheCurrentStageData();

	LR_INFO(TEXT("Stage Loaded: %s"), *CurrentStageData->StageName.ToString());
}

FStageStaticData UStageManagerSubsystem::GetCurrentStageDataCopy() const
{
	return *CurrentStageData;
}

const FStageStaticData* UStageManagerSubsystem::GetCurrentStateData()
{
	return CurrentStageData;
}

const FStageSpawnerData& UStageManagerSubsystem::GetCurrentStageSpawnerData()
{
	return CachedSpawnerData;
}

const FStageRewardData& UStageManagerSubsystem::GetCurrentStageRewardData()
{
	return CachedRewardData;
}

FName UStageManagerSubsystem::GetCurrentStageID() const
{
	if (!CurrentStageData)
	{
		LR_INFO(TEXT("CurrentStageData is null. Cannot retrieve StageID. Returning NAME_None."));
		return NAME_None;
	}
	return CurrentStageData->DataID;
}

const FString UStageManagerSubsystem::GetStageName() const
{
	if (!CurrentStageData)
	{
		LR_INFO(TEXT("CurrentStageData is null. Cannot retrieve stage name. Returning 'Unknown Stage'."));
		return TEXT("Unknown Stage");
	}
	return CurrentStageData->StageName.ToString();
}

bool UStageManagerSubsystem::IsBossStage() const
{
	if (!CurrentStageData)
	{
		LR_INFO(TEXT("CurrentStageData is null. Cannot determine if it's a boss stage. Returning false."));
		return false;
	}
	return CurrentStageData->bIsBossStage;
}

FName UStageManagerSubsystem::GetBossEnemyID() const
{
	if (!CurrentStageData)
	{
		LR_WARN(TEXT("CurrentStageData가 존재하지 않습니다!"));
		return NAME_None;
	}
	return CurrentStageData->BossEnemyID;
}

void UStageManagerSubsystem::CacheCurrentStageData()
{
	CachedSpawnerData.SetByStaticData(*CurrentStageData);
	CachedRewardData.SetByStaticData(*CurrentStageData);
}

void FStageSpawnerData::SetByStaticData(const FStageStaticData& InData)
{
	SpawnableEnemies.Empty();
	
	int32 SpawnEnemyCount = InData.SpawnEnemyIDs.Num();
	int32 SpawnWeightCount = InData.SpawnWeights.Num();

	if(SpawnEnemyCount == SpawnWeightCount)
	{
		for(int32 i = 0; i < SpawnEnemyCount; ++i)
		{
			FStageSpawnEnemyData EnemyData;
			EnemyData.EnemyID = InData.SpawnEnemyIDs[i];
			EnemyData.SpawnWeight = InData.SpawnWeights[i];
			SpawnableEnemies.Add(EnemyData);
		}

		SpawnInterval = InData.SpawnInterval;
	}
	else
	{
		LR_SCREEN_INFO(TEXT("Mismatch in SpawnEnemyIDs and SpawnWeights count for Stage: %s"), *InData.StageName.ToString());
	}
}

void FStageRewardData::SetByStaticData(const FStageStaticData& InData)
{
	Gold = InData.RewardGold;
	NormalTicket = InData.RewardNormalTicket;
	EnhanceTicket = InData.RewardEnhanceTicket;
}