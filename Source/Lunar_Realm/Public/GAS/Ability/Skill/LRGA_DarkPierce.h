// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_DarkPierce.generated.h"

//============================================================================
/**
 * 관통형 투사체 GA
 * - LRPierceProjectile 사용
 * - 상태이상 없음. 순수 데미지
 * - DT_SkillEffect : EFFECT_PIERCE 참조
 */
//============================================================================
// (260228) KHS 제작. 제반 사항 구현.
//============================================================================

class ALRProjectile;
class UGameplayEffect;

UCLASS()
class LUNAR_REALM_API ULRGA_DarkPierce : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_DarkPierce();

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

	//DT참조키
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_PIERCE";
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_PIERCE";
	
};
