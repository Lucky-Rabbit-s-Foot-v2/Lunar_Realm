// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GAS/Tags/LRGameplayTags.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;

	// static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_AutoCombat.BT_AutoCombat'"));

	// UE 5.6의 FObjectFinder는 TObjectPtr 호환 문제가 있어 FSoftObjectPath를 사용
	static const FSoftObjectPath BTPath(
		TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_AutoCombat.BT_AutoCombat'"));  // ← 실제 경로로 변경
	BehaviorTreeAsset = Cast<UBehaviorTree>(BTPath.TryLoad());

	// 스폰 자체가 안되고 있는 상황
	if (BehaviorTreeAsset)
	{
		LR_WARN(TEXT("[%s] : BehaviorTreeAsset 로드 성공!!"), *GetName());
	}
	else
	{
		LR_ERROR(TEXT("[%s] : BehaviorTreeAsset 로드 실패!!"), *GetName());
	}
}