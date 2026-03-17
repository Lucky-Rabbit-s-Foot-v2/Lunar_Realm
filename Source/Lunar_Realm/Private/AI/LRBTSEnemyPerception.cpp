// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTSEnemyPerception.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
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
		LR_ERROR(TEXT("[%s] : No Valid AIController!"), *GetName());
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		LR_ERROR(TEXT("[%s] : No Valid BlackBoardComponent!"), *GetName());
		return;
	}

	// 공통 로직 호출
	UpdateHostileTarget(BB, AIController);

	// Enemy 전용: CoreInRange 갱신
	APawn* MyPawn = AIController->GetPawn();
	AActor* CoreActor = Cast<AActor>(BB->GetValueAsObject(LRBBKeys::TargetCore));
	if (MyPawn && CoreActor)
	{
		FVector MyLoc = MyPawn->GetActorLocation();
		FVector TargetLoc = CoreActor->GetActorLocation();

		float EnemyRadius = 0.f;
		if (UCapsuleComponent* CapsuleComp = MyPawn->FindComponentByClass<UCapsuleComponent>())
		{
			EnemyRadius = CapsuleComp->GetScaledCapsuleRadius();
		}

		FVector TargetClosestLoc = TargetLoc;
		if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(CoreActor->GetRootComponent()))
		{
			TargetCollision->GetClosestPointOnCollision(MyLoc, TargetClosestLoc);
		}

		const float DistToCore = FMath::Max(0.f, FVector::Dist(MyLoc, TargetClosestLoc) - EnemyRadius);

		BB->SetValueAsBool(LRBBKeys::CoreInRange, DistToCore <= (AIController->GetAttackRange() + 10.f));
	}
}
