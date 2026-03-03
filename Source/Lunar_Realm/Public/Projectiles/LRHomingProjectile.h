// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/LRProjectile.h"
#include "LRHomingProjectile.generated.h"

//============================================================================
/**
 * 유도형 투사체 클래스
 * - LockRange 내 가장 가까운 적을 타겟으로 지정
 * - HomingTargetComponent 기반 자동 유도
 * - 타겟 없으면 Linear로 전환
 * - 타격/소멸: 베이스 처리 (Single + OnHit)
 */
//============================================================================
// (260301) KHS 제작. 제반 사항 구현.
//============================================================================
UCLASS()
class LUNAR_REALM_API ALRHomingProjectile : public ALRProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnSkillObjectInitialized() override;
	virtual void OnPoolDeactivate_Implementation() override;

private:
	AActor* FindNearestHostile(FGameplayTag HostileTag, float SearchRadius) const;
	
};
