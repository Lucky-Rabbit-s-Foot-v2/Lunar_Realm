// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTask_MoveToTarget.h"
#include "Units/LRAIController.h"

ULRBTTask_MoveToTarget::ULRBTTask_MoveToTarget()
{
	NodeName = TEXT("LR Move To Target");
}

EBTNodeResult::Type ULRBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ALRAIController* AIController = Cast<ALRAIController>(OwnerComp.GetAIOwner());

	// TEST
	LR_INFO(TEXT("[MoveToTarget] AIController: %s"),
		AIController ? *AIController->GetClass()->GetName() : TEXT("Cast 실패"));
	LR_INFO(TEXT("[MoveToTarget] AttackRange: %f"),
		AIController ? AIController->GetAttackRange() : -1.f);

	if (AIController)
	{
		AcceptableRadius = FMath::Max(
			AIController->GetAttackRange() - AcceptanceRangeOffset, 10.f);
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
