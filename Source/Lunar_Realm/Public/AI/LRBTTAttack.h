// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
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
 // (260303) BJM 수정, TryAttackTarget() 호출로 변경
 // (260312) KWB 수정. Enemy 종류 세분화에 따른 엔진에서 타겟 설정용 멤버 추가
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

public:
	// 엔진에서 직접 타겟 설정할 용도의 멤버
	UPROPERTY(EditAnywhere, Category = "LR|Attack")
	FBlackboardKeySelector TargetKey;
};
