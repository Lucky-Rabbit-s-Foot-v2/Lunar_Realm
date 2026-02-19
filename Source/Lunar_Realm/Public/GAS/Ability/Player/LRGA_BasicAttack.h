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
// (260219) KHS GA동작방식 변경. 생성자 수정.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_BasicAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_BasicAttack();
	//260219 KHS 베이스GA동작방식 변경으로 상속함수 변경
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;


};
