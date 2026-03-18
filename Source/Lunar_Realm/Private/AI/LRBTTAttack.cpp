// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTTAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"


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

	float EnemyRadius = 0.f;
	if (UCapsuleComponent* CapsuleComp = MyPawn->FindComponentByClass<UCapsuleComponent>())
	{
		EnemyRadius = CapsuleComp->GetScaledCapsuleRadius();
	}

	FVector TargetClosestLoc = TargetLoc;
	if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
	{
		TargetCollision->GetClosestPointOnCollision(MyLoc, TargetClosestLoc);
	}

	const float DistToTarget = FMath::Max(0.f, FVector::Dist(MyLoc, TargetClosestLoc) - EnemyRadius);
	const float CurrentAttackRange = AIController->GetAttackRange();

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

	FLRBTAttackTaskMemory* Memory = CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);
	Memory->BTComp = &OwnerComp;
	Memory->ASC = ASC;
	Memory->bAbilityEndedSynchronously = false;
	Memory->bAbilityFailedSynchronously = false;

	UnregisterDelegate(Memory);

	Memory->AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &ULRBTTAttack::OnAbilityEnded, &OwnerComp, NodeMemory);
	Memory->AbilityFailedHandle = ASC->AbilityFailedCallbacks.AddUObject(this, &ULRBTTAttack::OnAbilityFailed, &OwnerComp, NodeMemory);
	
	Memory->ActivatedAbilityTag = AIController->TryAttackTarget(TargetActor);

	// GA가 동기적으로 이미 종료된 경우 처리
	if (Memory->bAbilityFailedSynchronously || (!Memory->ActivatedAbilityTag.IsValid() && !Memory->CooldownTagHandle.IsValid()))
	{
		UnregisterDelegate(Memory);
		return EBTNodeResult::Failed;
	}

	// GA가 동기적으로 이미 종료된 경우 처리
	if (Memory->bAbilityEndedSynchronously)
	{
		UnregisterDelegate(Memory);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type ULRBTTAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FLRBTAttackTaskMemory* Memory = CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);
	UnregisterDelegate(Memory);

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void ULRBTTAttack::OnAbilityEnded(const FAbilityEndedData& EndedData, UBehaviorTreeComponent* BTComp, uint8* NodeMemory)
{
	if (!EndedData.AbilityThatEnded)
	{
		return;
	}

	FLRBTAttackTaskMemory* Memory = reinterpret_cast<FLRBTAttackTaskMemory*>(NodeMemory);

	// 태그가 아직 설정 안됨 = GA가 동기적으로 종료된 경우
	if (!Memory->ActivatedAbilityTag.IsValid())
	{
		Memory->bAbilityEndedSynchronously = true;
		return;
	}

	if (EndedData.AbilityThatEnded->GetAssetTags().HasTag(Memory->ActivatedAbilityTag))
	{
		UnregisterDelegate(Memory);
		if (BTComp)
		{
			FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
		}
	}
}

void ULRBTTAttack::OnAbilityFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailureTags, UBehaviorTreeComponent* BTComp, uint8* NodeMemory)
{
	if (!FailedAbility) return;

	FLRBTAttackTaskMemory* Memory = CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);
	if (!Memory->ASC.IsValid())
	{
		return;
	}

	const FGameplayTagContainer* CooldownTags = FailedAbility->GetCooldownTags();

	// 쿨다운으로 인한 실패인 경우
	if (CooldownTags && !CooldownTags->IsEmpty() && FailureTags.HasAny(*CooldownTags))
	{
		Memory->CooldownTag = CooldownTags->First();

		// 쿨다운 태그가 제거될 때까지 감지 등록
		Memory->CooldownTagHandle = Memory->ASC->RegisterGameplayTagEvent(Memory->CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ULRBTTAttack::OnCooldownTagChanged, BTComp, NodeMemory);

		// InProgress 유지 (FinishLatentTask 호출 안 함)
		return;
	}

	// 동기적 실행 도중 실패한 경우 플래그만 세팅
	if (!Memory->ActivatedAbilityTag.IsValid())
	{
		Memory->bAbilityFailedSynchronously = true;
		return;
	}

	// 쿨다운이 아닌 다른 이유로 실패
	UnregisterDelegate(Memory);
	if (BTComp) FinishLatentTask(*BTComp, EBTNodeResult::Failed);
}

void ULRBTTAttack::OnCooldownTagChanged(const FGameplayTag Tag, int32 Count, UBehaviorTreeComponent* BTComp, uint8* NodeMemory)
{
	if (Count > 0)
	{
		// 쿨다운 태그 추가됨 -> 무시
		return;
	}

	// 쿨다운 종료
	FLRBTAttackTaskMemory* Memory = CastInstanceNodeMemory<FLRBTAttackTaskMemory>(NodeMemory);

	if (Memory->ASC.IsValid() && Memory->CooldownTagHandle.IsValid())
	{
		Memory->ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).Remove(Memory->CooldownTagHandle);
		Memory->CooldownTagHandle.Reset();
	}

	UnregisterDelegate(Memory);

	// Failed가 아닌 Succeeded를 반환하여 BT가 정상적으로 다시 시작되도록 수정
	if (BTComp) FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

void ULRBTTAttack::UnregisterDelegate(FLRBTAttackTaskMemory* Memory)
{
	if (Memory->ASC.IsValid())
	{
		if (Memory->AbilityEndedHandle.IsValid())
		{
			Memory->ASC->OnAbilityEnded.Remove(Memory->AbilityEndedHandle);
			Memory->AbilityEndedHandle.Reset();
		}
		if (Memory->AbilityFailedHandle.IsValid())
		{
			Memory->ASC->AbilityFailedCallbacks.Remove(Memory->AbilityFailedHandle);
			Memory->AbilityFailedHandle.Reset();
		}
		if (Memory->CooldownTagHandle.IsValid() && Memory->CooldownTag.IsValid())
		{
			Memory->ASC->RegisterGameplayTagEvent(Memory->CooldownTag, EGameplayTagEventType::NewOrRemoved).Remove(Memory->CooldownTagHandle);
			Memory->CooldownTagHandle.Reset();
		}
	}
}

uint16 ULRBTTAttack::GetInstanceMemorySize() const
{
	return sizeof(FLRBTAttackTaskMemory);
}

void ULRBTTAttack::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FLRBTAttackTaskMemory>(NodeMemory, InitType);
}

void ULRBTTAttack::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FLRBTAttackTaskMemory>(NodeMemory, CleanupType);
}
