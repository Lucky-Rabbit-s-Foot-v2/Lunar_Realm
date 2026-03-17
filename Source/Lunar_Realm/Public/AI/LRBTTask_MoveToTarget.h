// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "LRBTTask_MoveToTarget.generated.h"

/**
 *  MoveTo 상속 받은 에너미용 AttackRange <-> AcceptanceRadius 연동 Task
 */

 //=============================================================================
 // (260313) KWB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRBTTask_MoveToTarget : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	ULRBTTask_MoveToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	static constexpr float AcceptanceRangeOffset = 5.0f;
};
