// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Skill/PlayerSkill/LRGA_ArmySkill.h"
#include "LRGA_PioneerSkill.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGA_PioneerSkill : public ULRGA_ArmySkill
{
	GENERATED_BODY()
	

public:
	ULRGA_PioneerSkill();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled) override;

private:
	FTimerHandle SecondStrikeTimer;
	FTimerHandle ThirdStrikeTimer;
};
