// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_Heal.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260209) BJM 제작. Heal Ability 생성.
// (260219) KHS GA동작방식 변경. 생성자 수정.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_Heal : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	ULRGA_Heal();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayTagContainer* InSourceTags, const FGameplayTagContainer* InTargetTags, OUT FGameplayTagContainer* OutOptionalRelevantTags) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Ability")
	TSubclassOf<class UGameplayEffect> HealEffectClass;

//	//260219 KHS 베이스GA동작방식 변경으로 상속함수 변경
//	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
//	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
//
//protected:
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
//	TSubclassOf<UGameplayEffect> HealEffectClass;
//
//protected:
//	virtual const FGameplayTagContainer* GetCooldownTags() const override;
//
//protected:
//	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
};
