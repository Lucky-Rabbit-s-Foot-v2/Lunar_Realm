// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_LightningStrike.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGA_LightningStrike : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
public:
	ULRGA_LightningStrike();

protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	// 가장 가까운 적대 액터 탐색
	AActor* FindNearestHostile(FGameplayTag HostileTag, float SearchRadius) const;
	// 범위 내 적대 액터들에게 데미지 + 상태이상 적용
	void ApplyLightningDamage(FVector StrikeLocation, FGameplayTag HostileTag, float Damage);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> StatusEffectClass; // GE_Shock

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_LIGHTNING";

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_LIGHTNING";
};
