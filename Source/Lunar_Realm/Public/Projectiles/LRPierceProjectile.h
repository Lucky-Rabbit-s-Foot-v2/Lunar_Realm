// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/LRProjectile.h"
#include "LRPierceProjectile.generated.h"

//============================================================================
/**
 * 관통형 투사체 클래스
 * - 직선 비행 (LRLinearProjectile과 동일)
 * - 충돌 대상에게 데미지 후 관통 카운트 차감
 * - DamageDecay만큼 관통할수록 데미지 감소
 * - PierceCount 소진 시 풀 복귀
 * - 한 번 충돌한 대상은 이후 충돌 무시
 * - 전용 테이블: DT_Flight_Pierce
 */
//============================================================================
// (260228) KHS 제작. 제반 사항 구현.
//============================================================================
UCLASS()
class LUNAR_REALM_API ALRPierceProjectile : public ALRProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnSkillObjectInitialized() override;
	// false 반환 = 자식이 직접 GE 적용 + 풀 복귀 처리
	virtual bool OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit) override;
    
	virtual void OnPoolDeactivate_Implementation() override;

private:
	int32 CachedPierceCount;
	float CachedDamageDecay;
	float CachedCurrentDamage;  // 관통마다 감쇠된 현재 데미지
    
	TSet<AActor*> HitActors;    // 이미 충돌한 액터 추적
};
