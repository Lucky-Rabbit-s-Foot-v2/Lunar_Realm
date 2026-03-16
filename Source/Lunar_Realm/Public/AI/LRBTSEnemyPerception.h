// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/LRBTSPerception.h"
#include "LRBTSEnemyPerception.generated.h"

/**
 * Enemy 전용 Behavior Tree Service 클래스
 */

 //=============================================================================
 // (260312) KWB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRBTSEnemyPerception : public ULRBTSPerception
{
	GENERATED_BODY()

public:
	ULRBTSEnemyPerception();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
