// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LRBTTAttack.generated.h"

// =============================================================================
/**
 * LRBTTAttack 테스크
 *
 * [동작]
 * 1. BB에서 TargetActor를 읽는다
 * 2. 공격 사거리(AttackRange) 안인지 확인한다
 * 3. 사거리 안이면 => AIController -> TryAttackTarget() 호출
 * 4. 타겟을 바라본 뒤 결과를 반환한다
 *
 */
 //=============================================================================
 // (260211) KWB 제작. 제반 사항 구현.
 // =============================================================================
UCLASS()
class LUNAR_REALM_API ULRBTTAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	ULRBTTAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
