// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"


ULRBTTAttack::ULRBTTAttack()
{
	NodeName = TEXT("LR Attack");
}

EBTNodeResult::Type ULRBTTAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ALRAIController* AIController = Cast<ALRAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		LR_WARN(TEXT("[%s] : No Valid AIController"), *GetName());
		return EBTNodeResult::Failed;
	}

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn)
	{
		LR_WARN(TEXT("[%s] : No Valid Owner Pawn!!"), *GetName());
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		LR_WARN(TEXT("[%s] : No Valid Black Board!"), *GetName());
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(LRBBKeys::TargetActor));
	if (!TargetActor)
	{
		LR_WARN(TEXT("[%s] : No Valid Target Actor Exist! => Check Level"), *GetName());
		return EBTNodeResult::Failed;
	}

	// 사거리 체크
	const float DistToTarget = FVector::Dist(
		MyPawn->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	const float CurrentAttackRange = AIController->GetAttackRange();

	if (DistToTarget > CurrentAttackRange)
	{
		// 사거리 밖 -> Sequence를 실패 -> MoveTo 실행
		return EBTNodeResult::Failed;
	}

	// TODO: 회전 자연스러운지 확인
	// 타겟 방향으로 회전
	FVector LookDir = (TargetActor->GetActorLocation() - MyPawn->GetActorLocation()).GetSafeNormal();
	LookDir.Z = 0.0f;
	if (!LookDir.IsNearlyZero())
	{
		MyPawn->SetActorRotation(LookDir.Rotation());
	}

	// 공격 실행
	// TryAttackTarget()이 false를 반환해도 (쿨타임) Succeeded로 처리
	AIController->TryAttackTarget(TargetActor);

	return EBTNodeResult::Succeeded;
}
