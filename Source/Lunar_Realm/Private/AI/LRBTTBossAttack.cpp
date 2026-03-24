// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTBossAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "Units/LRAIController.h"

ULRBTTBossAttack::ULRBTTBossAttack()
{
	NodeName = TEXT("LR Boss Attack");
}

EBTNodeResult::Type ULRBTTBossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// ── 부모 ExecuteTask의 전반부 로직 재현 (타겟 획득, 사거리 체크, 회전) ──
	ALREnemyAIController* EnemyAICtrl = Cast<ALREnemyAIController>(OwnerComp.GetAIOwner());
	if (!EnemyAICtrl)
	{
		LR_WARN(TEXT("[%s] : No Valid EnemyAIController"), *GetName());
		return EBTNodeResult::Failed;
	}

	APawn* MyPawn = EnemyAICtrl->GetPawn();
	if (!MyPawn)
	{
		LR_WARN(TEXT("[%s] : No Valid Owner Pawn!"), *GetName());
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		LR_WARN(TEXT("[%s] : No Valid BlackBoard!"), *GetName());
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
	{
		LR_WARN(TEXT("[%s] : No Valid Target Actor!"), *GetName());
		return EBTNodeResult::Failed;
	}

	// 사거리 체크
	FVector MyLoc = MyPawn->GetActorLocation();
	float EnemyRadius = 0.f;
	if (UCapsuleComponent* CapsuleComp = MyPawn->FindComponentByClass<UCapsuleComponent>())
	{
		EnemyRadius = CapsuleComp->GetScaledCapsuleRadius();
	}

	FVector TargetClosestLoc = TargetActor->GetActorLocation();
	if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
	{
		TargetCollision->GetClosestPointOnCollision(MyLoc, TargetClosestLoc);
	}

	const float DistToTarget = FMath::Max(0.f, FVector::Dist(MyLoc, TargetClosestLoc) - EnemyRadius);
	const float CurrentAttackRange = EnemyAICtrl->GetAttackRange();

	if (DistToTarget > CurrentAttackRange + 10.f)
	{
		return EBTNodeResult::Failed;
	}

	// 타겟 방향으로 회전
	FVector LookDir = (TargetActor->GetActorLocation() - MyPawn->GetActorLocation()).GetSafeNormal();
	LookDir.Z = 0.0f;
	if (!LookDir.IsNearlyZero())
	{
		MyPawn->SetActorRotation(LookDir.Rotation());
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyPawn);
	if (!ASC)
	{
		LR_WARN(TEXT("[%s] : No Valid ASC!"), *GetName());
		return EBTNodeResult::Failed;
	}

	// ── 보스 전용: Phase 기반 공격 ──
	const int32 CurrentPhase = BB->GetValueAsInt(LRBBKeys::CurrentPhase);
	FGameplayTag AttackTag = EnemyAICtrl->TryAttackTargetByPhase(TargetActor, CurrentPhase);

	// 부모의 헬퍼를 사용하여 델리게이트 등록 + InProgress 패턴 처리
	return FinishExecuteWithTag(OwnerComp, NodeMemory, ASC, AttackTag);
}
