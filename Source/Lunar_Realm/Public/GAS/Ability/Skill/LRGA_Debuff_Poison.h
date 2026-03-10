// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_Debuff_Poison.generated.h"

/**
 * ULRGA_Debuff_Poison
 * 독 디버프 GA. 가장 가까운 플레이어에게 Duration GE 적용 (틱마다 HP 감소).
 * Instigator = 에너미, Target = 가장 가까운 플레이어
 */
//=============================================================================
// (260309) KHS 제작
//=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_Debuff_Poison : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
public:
	ULRGA_Debuff_Poison();

protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	// 에디터에서 GE_Debuff_Poison 에셋 지정
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> PoisonEffectClass;
	
};
