// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_BasicAttack.generated.h"

/**
 *  기본 평타 공격 어빌리티
 */

//=============================================================================
// (260209) BJM 제작. Attack Ability 생성.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_BasicAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_BasicAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;


};
