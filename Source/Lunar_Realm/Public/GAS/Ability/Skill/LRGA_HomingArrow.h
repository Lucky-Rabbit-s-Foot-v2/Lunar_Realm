// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_HomingArrow.generated.h"

//============================================================================
/**
 * 유도형 투사체 GA
 * - LRHomingProjectile 사용
 * - 상태이상: Shock
 * - DT_SkillEffect : EFFECT_LIGHTNING 참조
 */
//============================================================================
// (260301) KHS 제작. 제반 사항 구현.
//============================================================================

class ALRProjectile;
class UGameplayEffect;

UCLASS()
class LUNAR_REALM_API ULRGA_HomingArrow : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_HomingArrow();

protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<ALRProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_HOMING_ARROW";
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_HOMING_ARROW";
};
