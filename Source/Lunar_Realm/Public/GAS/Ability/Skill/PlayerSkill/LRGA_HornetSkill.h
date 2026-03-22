// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_HornetSkill.generated.h"

/**
 * 
 */

class UNiagaraComponent;

UCLASS()
class LUNAR_REALM_API ULRGA_HornetSkill : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_HornetSkill();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled) override;

	UFUNCTION()
	void TickDamage();

	UFUNCTION()
	void FinishSkill();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	float BeamLength = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	float BeamRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	float TickRate = 0.5f;

private:
	FTimerHandle DamageTimerHandle;
	FTimerHandle DurationTimerHandle;

	UPROPERTY()
	UNiagaraComponent* SpawnedBeamVFX;

};
