// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_InstantHeal.generated.h"

/**
 * ULRGA_InstantHeal
 * 즉시 HP 회복 GA. Self에게 Instant GE 적용.
 * Instigator = 플레이어 자신
 */
//=============================================================================
// (260309) KHS 제작
//=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_InstantHeal : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
    ULRGA_InstantHeal();

protected:
    virtual void OnAbilityActivated(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
    // 에디터에서 GE_Heal_Instant 에셋 지정
    UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
    TSubclassOf<UGameplayEffect> HealEffectClass;

	// DT_SkillStaticData의 RowName
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_INSTANT_HEAL";
    // DT_SkillEffect의 RowName
    UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
    FName SkillEffectID = "EFFECT_INSTANT_HEAL";
};
