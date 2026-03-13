// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTask_MoveToTarget.h"
#include "Units/LRAIController.h"

ULRBTTask_MoveToTarget::ULRBTTask_MoveToTarget()
{
	NodeName = TEXT("LR Move To Target");
}

EBTNodeResult::Type ULRBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type ULRBTTask_MoveToTarget::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ALRAIController* AIController = Cast<ALRAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		AcceptableRadius = FMath::Max(
			AIController->GetAttackRange() - AcceptanceRangeOffset, 10.f);
	}
	return Super::PerformMoveTask(OwnerComp, NodeMemory);
}