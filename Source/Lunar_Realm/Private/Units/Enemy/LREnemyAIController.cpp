// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GAS/Tags/LRGameplayTags.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;

	// TODO: StaticData에서 읽어오는걸로 리팩토링 필요
	static const FSoftObjectPath BTPath(
		TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_AutoCombat.BT_AutoCombat'"));
	BehaviorTreeAsset = Cast<UBehaviorTree>(BTPath.TryLoad());

	//if (BehaviorTreeAsset)
	//{
	//	LR_WARN(TEXT("[%s] : BehaviorTreeAsset 로드 성공!!"), *GetName());
	//}
	//else
	//{
	//	LR_ERROR(TEXT("[%s] : BehaviorTreeAsset 로드 실패!!"), *GetName());
	//}
}