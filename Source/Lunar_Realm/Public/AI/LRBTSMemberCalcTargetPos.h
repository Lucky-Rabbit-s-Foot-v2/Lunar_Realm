// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "LRBTSMemberCalcTargetPos.generated.h"

/**
 * 260331 BJM 생성
 */
UCLASS()
class LUNAR_REALM_API ULRBTSMemberCalcTargetPos : public UBTService
{
	GENERATED_BODY()
	
public:
	ULRBTSMemberCalcTargetPos();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

};
