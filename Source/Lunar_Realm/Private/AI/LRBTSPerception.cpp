// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTSPerception.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"


ULRBTSPerception::ULRBTSPerception()
{
	NodeName = TEXT("LR Perception");

	Interval = 0.3f;
	RandomDeviation = 0.05f; // 약간의 랜덤 편차로 모든 AI가 동시에 갱신되는 것을 방지
}

void ULRBTSPerception::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ALRAIController* AIController = Cast<ALRAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		LR_WARN(TEXT("[%s] : No Valid AIController"), *GetName());
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		LR_WARN(TEXT("[%s] : No Valid BB"), *GetName());

		return;
	}

	UpdateHostileTarget(BB, AIController);
}

void ULRBTSPerception::UpdateHostileTarget(UBlackboardComponent* BB, ALRAIController* AIController)
{
	// 적 캐릭터 추적 중, 타겟이 사망시 즉시 BB 클리어 => 뒤돌아 보는 버그 수정
	if (BB->GetValueAsBool(LRBBKeys::HasNearbyHostile))
	{
		AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(LRBBKeys::TargetActor));
		if (CurrentTarget)
		{
			bool bShouldDrop = false;

			if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(CurrentTarget))
			{
				if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
				{
					if (ASC->HasMatchingGameplayTag(LRTags::State_Dead))
					{
						bShouldDrop = true;
					}
				}
			}

			if (bShouldDrop)
			{
				BB->SetValueAsObject(LRBBKeys::TargetActor, nullptr);
				BB->SetValueAsBool(LRBBKeys::HasNearbyHostile, false);
				AIController->StopMovement();
			}
		}
	}

	if (BB->GetValueAsObject(LRBBKeys::TargetCore) == nullptr)
	{
		AActor* CoreActor = AIController->FindTargetCore();
		BB->SetValueAsObject(LRBBKeys::TargetCore, CoreActor);
	}

	AActor* NearestHostile = AIController->FindNearestHostile();

	// 적대 캐릭터 있으면 타겟으로 설정, 없을 시 코어를 타겟으로 설정
	if (NearestHostile)
	{
		BB->SetValueAsObject(LRBBKeys::TargetActor, NearestHostile);
		BB->SetValueAsBool(LRBBKeys::HasNearbyHostile, true);
	}
	else
	{
		AActor* CoreTarget = Cast<AActor>(BB->GetValueAsObject(LRBBKeys::TargetCore));
		BB->SetValueAsObject(LRBBKeys::TargetActor, CoreTarget);
		BB->SetValueAsBool(LRBBKeys::HasNearbyHostile, false);
	}
}

