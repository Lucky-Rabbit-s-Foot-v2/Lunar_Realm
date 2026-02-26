// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameState.h"

#include "Engine/GameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"

void ALRStageGameState::BeginPlay()
{
	Super::BeginPlay();

	UStageManagerSubsystem* StageManager = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();

	const FStageSpawnerData& SpawnerData = StageManager->GetCurrentStageSpawnerData();

	LR_SCREEN_INFO(TEXT("Current Stage Spawner Data: Spawnable Enemies Count = %d, Spawn Interval = %f"), SpawnerData.SpawnableEnemies.Num(), SpawnerData.SpawnInterval);


	const FStageRewardData& RewardData = StageManager->GetCurrentStageRewardData();
	LR_SCREEN_INFO(TEXT("Current Stage Reward Data: Gold = %d, Normal Tickets = %d, Enhance Tickets = %d"), RewardData.Gold, RewardData.NormalTicket, RewardData.EnhanceTicket);

}
