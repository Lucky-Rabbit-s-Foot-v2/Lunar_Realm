// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_Iceball.generated.h"

//============================================================================
/**
 * 곡사형 투사체 GA
 * - LRArcProjectile 사용
 * - 상태이상: Freeze
 * - DT_SkillEffect : EFFECT_ICEBALL 참조
 */
//============================================================================
// (260228) KHS 제작. 제반 사항 구현.
//============================================================================

class ALRProjectile;
class UGameplayEffect;

UCLASS()
class LUNAR_REALM_API ULRGA_Iceball : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_Iceball();

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
	TSubclassOf<UGameplayEffect> StatusEffectClass; // GE_Freeze

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_ICEBALL";
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_ICEBALL";
	
};
