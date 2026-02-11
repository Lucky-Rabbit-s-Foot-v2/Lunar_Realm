// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTSPerception.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
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
