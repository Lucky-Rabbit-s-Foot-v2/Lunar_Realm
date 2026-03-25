// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/LRBTTAttack.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "LRBTTBossAttack.generated.h"

/**
 * 보스 전용 공격 태스크
 * - BB의 CurrentPhase 통해서 페이즈 결정
 *   Phase 0 → SkillIDs[0~1], Phase 1 → SkillIDs[2~3], Phase 2 → SkillIDs[4~5] 예상
 */

 //============================================================================
 // (260312) KWB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRBTTBossAttack : public ULRBTTAttack
{
	GENERATED_BODY()

public:
	ULRBTTBossAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
