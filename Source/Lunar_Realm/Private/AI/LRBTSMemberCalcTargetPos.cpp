// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LRBTSMemberCalcTargetPos.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"

ULRBTSMemberCalcTargetPos::ULRBTSMemberCalcTargetPos()
{
	NodeName = TEXT("Calculate Surface Target Pos");
	bNotifyTick = true;
}

void ULRBTSMemberCalcTargetPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();

	if (!BBComp || !AICon) return;

	APawn* MyPawn = AICon->GetPawn();
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (MyPawn && TargetActor)
	{
		FVector MyLoc = MyPawn->GetActorLocation();
		FVector TargetLoc = TargetActor->GetActorLocation();

		if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
		{
			TargetCollision->GetClosestPointOnCollision(MyLoc, TargetLoc);
		}

		TargetLoc.Z = MyLoc.Z;

		BBComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, TargetLoc);
	}

}
