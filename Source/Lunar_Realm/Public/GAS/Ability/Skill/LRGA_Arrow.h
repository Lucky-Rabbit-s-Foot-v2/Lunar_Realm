// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_Arrow.generated.h"

//============================================================================
/**
 * 직선형 투사체 클래스
 * - LRLinearProjectile 사용
 * - 상태이상 없음. 순수 데미지
 * - DT_SkillEffect : EFFECT_ARROW참조
 */
//============================================================================
// (260224) KHS 제작. 제반 사항 구현.
//============================================================================

class ALRProjectile;
class UGameplayEffect;

UCLASS()
class LUNAR_REALM_API ULRGA_Arrow : public ULRGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	ULRGA_Arrow();
	
protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	
protected:
	//에디터에서 할당
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<ALRProjectile> ProjectileClass;
	//데미지 GE
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	//DT 참조 키
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = "EFFECT_ARROW";
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = "SKILL_ARROW";
	
};
