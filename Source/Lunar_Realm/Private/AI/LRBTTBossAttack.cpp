// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTBossAttack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ULRBTTBossAttack::ULRBTTBossAttack()
{
	NodeName = TEXT("LR Boss Attack");
}

EBTNodeResult::Type ULRBTTBossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		LR_WARN(TEXT("[%s] : No Valid BlackBoard!"), *GetName());
		return EBTNodeResult::Failed;
	}

	const int32 CurrentPhase = BB->GetValueAsInt(LRBBKeys::CurrentPhase);

	// TODO: CurrentPhase에 따라 TryAttackTarget에 스킬 인덱스 전달
	// 스킬 시스템 리팩토링(TryAttackTarget → TryAttackTargetBySkillIndex) 이후 구현 -> 일반 에너미 스킬 사용 부분 수정 후 추가 수정 필요
	LR_INFO(TEXT("[BossAttack] 현재 페이즈: %d"), CurrentPhase);

	// 현재는 기본 공격 로직 사용
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
