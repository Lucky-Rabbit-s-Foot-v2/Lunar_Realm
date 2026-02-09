// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_Charge.generated.h"

/**
 * 
 */

//=============================================================================
// (260209) BJM 제작. Aether Charge Ability 생성.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_Charge : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_Charge();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> ChargeEffectClass;
	
};
