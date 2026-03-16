// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTSEnemyPerception.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ULRBTSEnemyPerception::ULRBTSEnemyPerception()
{
	NodeName = TEXT("LR Enemy Perception");

	Interval = 0.3f;
	RandomDeviation = 0.05f;
}

void ULRBTSEnemyPerception::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Super 의도적 미실행 => 중복 기능 방지

	UBTService::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ALRAIController* AIController = Cast<ALRAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	// 공통 로직 호출
	UpdateHostileTarget(BB, AIController);

	// Enemy 전용: CoreInRange 갱신
	APawn* MyPawn = AIController->GetPawn();
	AActor* CoreActor = Cast<AActor>(BB->GetValueAsObject(LRBBKeys::TargetCore));
	if (MyPawn && CoreActor)
	{
		const float DistToCore = FVector::Dist(MyPawn->GetActorLocation(), CoreActor->GetActorLocation());
		BB->SetValueAsBool(LRBBKeys::CoreInRange, DistToCore <= AIController->GetAttackRange());
	}
}
