// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_ArmySkill.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGA_ArmySkill : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_ArmySkill();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;

	void PerformStrikeLogic();

	UFUNCTION()
	void OnStrikeMontageFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	UAnimMontage* StrikeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	float CachedHitRadius;
	FName CachedResourceID;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	float StrikeDistance = 200.0f;

};
