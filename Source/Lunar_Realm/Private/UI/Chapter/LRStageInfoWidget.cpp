// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/TextBlock.h"

#include "UI/Chapter/LREnemyInfo.h"
#include "UI/Chapter/LRRewardInfoWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRStageInfoWidget::SetStageDataByID(FName InStageID)
{
	CurrentStageID = InStageID;

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const auto& StageData = GameDataSubsystem->GetStageStaticData(CurrentStageID);

	Txt_StageName->SetText(StageData.StageName);

	Enemy1->SetEnemyID(StageData.SpawnEnemyIDs.IsValidIndex(0) ? StageData.SpawnEnemyIDs[0] : NAME_None);
	Enemy2->SetEnemyID(StageData.SpawnEnemyIDs.IsValidIndex(1) ? StageData.SpawnEnemyIDs[1] : NAME_None);
	Enemy3->SetEnemyID(StageData.SpawnEnemyIDs.IsValidIndex(2) ? StageData.SpawnEnemyIDs[1] : NAME_None);

	Reward1->SetRewardAmount(StageData.RewardGold);
	Reward2->SetRewardAmount(StageData.RewardNormalTicket);
	Reward3->SetRewardAmount(StageData.RewardEnhanceTicket);
}

