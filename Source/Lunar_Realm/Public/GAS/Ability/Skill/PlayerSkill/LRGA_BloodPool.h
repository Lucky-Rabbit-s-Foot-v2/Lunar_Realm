// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_BloodPool.generated.h"

/**
 * 
 */
 //=============================================================================
 // (260320) BJM 제작. Nurse전용기 스킬 .
 //=============================================================================
class ALRAoEActor;
struct FSkillResourceData;
struct FSkillEffectData;
struct FSkillStaticData;

UCLASS()
class LUNAR_REALM_API ULRGA_BloodPool : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_BloodPool();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;

	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData InPayload);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	TSubclassOf<ALRAoEActor> AoEActorClass;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

protected:
	void SpawnSinglePool(FVector InSpawnLocation, const FSkillEffectData& InEffectData, const FSkillStaticData& InSkillData);

};
