// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/LRProjectile.h"
#include "LRArcProjectile.generated.h"

//============================================================================
/**
 * 곡사형 투사체 클래스
 * - LaunchAngle + GravityScale 기반 포물선 비행
 * - 착탄 또는 Lifetime 만료 시 폭발 (Area 데미지)
 * - 상태이상: Freeze
 * - 전용 테이블: DT_Flight_Arc
 */
//============================================================================
// (260228) KHS 제작. 제반 사항 구현.
//============================================================================

UCLASS()
class LUNAR_REALM_API ALRArcProjectile : public ALRProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnSkillObjectInitialized() override;
	// false 반환 = 자식이 직접 폭발 처리 + 풀 복귀
	virtual bool OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit) override;
	// Lifetime 만료 시 현재 위치에서 강제 폭발
	virtual void OnSkillObjectExpired() override;

private:
	void ApplyExplosionDamage();

	float CachedExplosionRadius;
	float CachedExplosionDamageMultiplier;
	
};
