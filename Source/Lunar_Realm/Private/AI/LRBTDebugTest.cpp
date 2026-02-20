// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTDebugTest.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ULRBTDebugTest::ULRBTDebugTest()
{
	NodeName = TEXT("LR BB Spy");
	Interval = 0.5f;
}

void ULRBTDebugTest::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	UObject* TargetObj = BB->GetValueAsObject(FName("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObj);
	UObject* CoreObj = BB->GetValueAsObject(FName("TargetCore"));
	bool bHasHostile = BB->GetValueAsBool(FName("HasNearbyHostile"));

	LR_INFO(TEXT("=== [BB Spy] %s ==="), *OwnerComp.GetOwner()->GetName());
	LR_INFO(TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
	LR_INFO(TEXT("TargetCore: %s"), CoreObj ? *CoreObj->GetName() : TEXT("NULL"));
	LR_INFO(TEXT("HasNearbyHostile: %s"), bHasHostile ? TEXT("TRUE") : TEXT("FALSE"));
}
