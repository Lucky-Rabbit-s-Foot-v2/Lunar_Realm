// Fill out your copyright notice in the Description page of Project Settings.

// TEMP
#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_InstantAttack.generated.h"

/**
 *  기본 평타 공격 어빌리티
 */

//=============================================================================
// (260209) KWB 제작. Attack Ability 생성.
// (260219) KHS GA동작방식 변경. 생성자 수정.
// (260223) KWB 쿨다운 기능 추가
// (260312) KWB OnAbilityActivated()의 "TargetActor" 값 저장 방식 변경 (Controller -> GameplayAbilityBase)
// (260313) KWB 중복 로직 제거 (DT를 통해 AttackRange 값 결정 및 BTTask에서 검사)
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_InstantAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_InstantAttack();

protected:
	//260219 KHS 베이스GA동작방식 변경으로 상속함수 변경
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	void UnbindMontageCallbacks(UAnimInstance* AnimInstance);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	TObjectPtr<UAnimMontage> ActiveMontage = nullptr;
	bool bDamageApplied = false;

	// 에디터의 AnimMontage에 설정해야 하는 Notify 이름
	static const FName HitNotifyName;
};
