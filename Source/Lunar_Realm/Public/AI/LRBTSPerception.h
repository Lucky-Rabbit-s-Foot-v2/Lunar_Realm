// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "LRBTSPerception.generated.h"

// =============================================================================
/**
 * LRBTSPerception 서비스
 * 
 * [동작]
 * 매 Tick(기본 0.3초)마다:
 * 1. AIController -> FindNearestHostile()로 탐지 범위 내 적대 캐릭터 검색
 * 2. 적대 캐릭터가 있으면 -> BB.TargetActor = 해당 캐릭터, HasNearbyHostile = true
 * 3. 없으면             -> BB.TargetActor = BB.TargetCore(코어), HasNearbyHostile = false
 * 
 */
 //=============================================================================
 // (260211) KWB 제작. 제반 사항 구현.
 // =============================================================================
UCLASS()
class LUNAR_REALM_API ULRBTSPerception : public UBTService
{
	GENERATED_BODY()
	
public:
	ULRBTSPerception();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
