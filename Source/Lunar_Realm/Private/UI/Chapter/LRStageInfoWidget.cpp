// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/TextBlock.h"

#include "UI/Chapter/LREnemyInfo.h"
#include "UI/Chapter/LRRewardInfoWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRStageInfoWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	EnemyInfos = { Enemy1, Enemy2, Enemy3 };
	RewardInfos = { Reward1, Reward2, Reward3 };
}

void ULRStageInfoWidget::SetStageDataByID(FName InStageID)
{
	CurrentStageID = InStageID;

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const auto& StageData = GameDataSubsystem->GetStageStaticData(CurrentStageID);

	Txt_StageName->SetText(StageData.StageName);

	for (int32 i = 0; i < EnemyInfos.Num(); ++i)
	{
		if (EnemyInfos[i])
		{
			FName EnemyID = StageData.SpawnEnemyIDs.IsValidIndex(i) ? StageData.SpawnEnemyIDs[i] : NAME_None;
			if(EnemyID == NAME_None)
			{
				EnemyInfos[i]->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				EnemyInfos[i]->SetVisibility(ESlateVisibility::Visible);
				EnemyInfos[i]->SetEnemyID(EnemyID);
			}
		}
	}
	Reward1->SetRewardAmount(StageData.RewardGold);
	Reward2->SetRewardAmount(StageData.RewardNormalTicket);
	Reward3->SetRewardAmount(StageData.RewardEnhanceTicket);
}

