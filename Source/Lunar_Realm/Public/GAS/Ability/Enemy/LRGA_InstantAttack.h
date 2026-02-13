// Fill out your copyright notice in the Description page of Project Settings.

// TEMP
#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_InstantAttack.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGA_InstantAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_InstantAttack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float AttackRange = 200.0f; // 공격 가능 거리
};
