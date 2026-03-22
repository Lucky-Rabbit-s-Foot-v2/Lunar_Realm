// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_RapidSlash.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260317) BJM 제작. Writer 전용기 스킬 .
 //=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_RapidSlash : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_RapidSlash();


protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) override;
	

	void StartNextStrike();
	void PerformStrikeLogic();


	UFUNCTION()
	void OnStrikeMontageFinished();

	// === 스킬 세팅 변수 ===
	UPROPERTY(EditDefaultsOnly, Category = "LR|Rapid Slash")
	class UAnimMontage* RapidMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Rapid Slash")
	int32 MaxStrikeCount = 5; // 총 타격 횟수

	UPROPERTY(EditDefaultsOnly, Category = "LR|Rapid Slash")
	TSubclassOf<UGameplayEffect> DamageEffectClass;


private:
	int32 CurrentStrikeCount = 0;

	float CachedDamage = 0.0f;
	float CachedHitRadius = 0.0f;
	FName CachedResourceID;

	TWeakObjectPtr<AActor> LockedTarget;
};
