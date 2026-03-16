// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"
#include "Components/PrimitiveComponent.h"


ULRBTTAttack::ULRBTTAttack()
{
	NodeName = TEXT("LR Attack");

	// Object 타입 키만 드롭다운에 표시되도록 필터 설정
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ULRBTTAttack, TargetKey), AActor::StaticClass());
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

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
	{
		LR_WARN(TEXT("[%s] : No Valid Target Actor Exist! => Check Level"), *GetName());
		return EBTNodeResult::Failed;
	}

	// BJM 타겟 사거리 계산 수정
	FVector MyLoc = MyPawn->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();

	if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
	{
		TargetCollision->GetClosestPointOnCollision(MyLoc, TargetLoc);
	}

	const float DistToTarget = FVector::Dist(MyLoc, TargetLoc);
	const float CurrentAttackRange = AIController->GetAttackRange();

	if (DistToTarget > CurrentAttackRange)
	{
		// 사거리 밖 -> Sequence를 실패 -> MoveTo 실행
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

	FLRBTAttackTaskMemory* Memory = CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);
	Memory->BTComp = &OwnerComp;
	Memory->ASC = ASC;

	Memory->AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &ULRBTTAttack::OnAbilityEnded, &OwnerComp, NodeMemory);
	
	Memory->ActivatedAbilityTag = AIController->TryAttackTarget(TargetActor);

	// [변경] Succeeded -> InProgress (BT 루프 방지)
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type ULRBTTAttack::AbortTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FLRBTAttackTaskMemory* Memory =
		CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);
	UnregisterDelegate(Memory);

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void ULRBTTAttack::OnAbilityEnded(const FAbilityEndedData& EndedData, UBehaviorTreeComponent* BTComp, uint8* NodeMemory)
{
	if (!EndedData.AbilityThatEnded)
	{
		return;
	}

	FLRBTAttackTaskMemory* Memory =
		CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);

	// 발동했던 GA가 아니면 무시
	if (!EndedData.AbilityThatEnded->GetAssetTags().HasTag(Memory->ActivatedAbilityTag))
	{
		return;
	}

	UnregisterDelegate(Memory);

	if (BTComp)
	{
		FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
	}
}

void ULRBTTAttack::UnregisterDelegate(FLRBTAttackTaskMemory* Memory)
{
	if (Memory->ASC.IsValid() && Memory->AbilityEndedHandle.IsValid())
	{
		Memory->ASC->OnAbilityEnded.Remove(Memory->AbilityEndedHandle);
		Memory->AbilityEndedHandle.Reset();
	}
}

uint16 ULRBTTAttack::GetInstanceMemorySize() const
{
	return sizeof(FLRBTAttackTaskMemory);
}

void ULRBTTAttack::InitializeMemory(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FLRBTAttackTaskMemory>(NodeMemory, InitType);
}

void ULRBTTAttack::CleanupMemory(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FLRBTAttackTaskMemory>(NodeMemory, CleanupType);
}
