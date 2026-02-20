// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "LRBTDebugTest.generated.h"

/**
 * 블랙보드의 특정 키 값들을 실시간으로 로그에 출력하는 디버그용
 */

 //=============================================================================
 // (260217) BJM 제작.
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRBTDebugTest : public UBTService
{
	GENERATED_BODY()
	
public:
	ULRBTDebugTest();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
