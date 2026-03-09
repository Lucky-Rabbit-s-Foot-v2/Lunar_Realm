// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_DurationalHeal.generated.h"


/**
 * ULRGA_DurationHeal
 * 지속 HP 회복 GA. Self에게 Duration GE 적용 (틱마다 회복).
 * Instigator = 플레이어 자신
 */
//=============================================================================
// (260309) KHS 제작
//=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_DurationalHeal : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_DurationalHeal();

protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> HealEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_DURATION_HEAL";
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_DURATION_HEAL";
};
