// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_HomingFairyArrow.generated.h"

/**
 * 
 */
 //=============================================================================
 // (260320) BJM 제작. Muzzle전용기 스킬 .
 //=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_HomingFairyArrow : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_HomingFairyArrow();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;

	void SpawnDistributedArrows(TSubclassOf<ALRProjectile> InProjectileClass, const FSkillObjectInitData& InInitData);


protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<ALRProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
