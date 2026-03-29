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
// (260219) BJM GA동작방식 수정
// (260224) BJM 애님 노티파이 추가
// (260329) KWB LRGameplayAbilityBase 멤버 타입 전환에 따른 코드 수정 : CachedTarget -> CachedTarget.Get()
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
	// 몽타주에서 날아온 타격 이벤트를 받았을 때 실행될 함수
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData InPayload);

	// 몽타주 재생이 완전히 끝났을 때 어빌리티를 종료할 함수
	UFUNCTION()
	void OnMontageEnded();
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Basic Attack")
	bool bIsMeleeAttack = true;

	UPROPERTY(EditAnywhere, Category = "LR|Basic Attack|Ranged", meta = (EditCondition = "!bIsMeleeAttack"))
	TSubclassOf<class ALRProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "LR|Basic Attack|Ranged", meta = (EditCondition = "!bIsMeleeAttack"))
	float ProjectileSpeed = 1500.0f;

	void PlayAttackGruntSound(class ALRCharacter* InOwnerChar);

	//UPROPERTY(EditDefaultsOnly, Category = "LR|Basic Attack|Data")
	//FName BasicAttackSkillID;

	//UPROPERTY(EditDefaultsOnly, Category = "LR|Basic Attack|Data")
	//FName BasicAttackSkillEffectID;

};
