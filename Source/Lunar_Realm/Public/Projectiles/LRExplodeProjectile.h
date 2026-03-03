// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/LRProjectile.h"
#include "LRExplodeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRExplodeProjectile : public ALRProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnSkillObjectInitialized() override;
	// true반환 = 베이스가 풀복귀, false반환 = 자식이 직접 처리
	virtual bool OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit) override;
	virtual void OnSkillObjectExpired() override;
	
private:
	void ApplyExplosionDamage();

	float CachedExplosionRadius;
	float CachedExplosionDamageMultiplier;
};
