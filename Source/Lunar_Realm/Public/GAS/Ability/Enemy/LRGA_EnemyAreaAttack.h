#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "LRGA_EnemyAreaAttack.generated.h"

/**
 * 에너미 근접 범위 공격 GA
 * - 몽타주 노티파이 시점에 에너미 구형 범위 내 적대 대상 전체에 데미지 적용
 * - 범위와 VFX/SFX는 BP Class Defaults에서 에너미별로 세팅
 */
 // ============================================================================
 // (260323) KWB 제작.
 // ============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_EnemyAreaAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_EnemyAreaAttack();

protected:
	virtual void OnAbilityActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void UnbindMontageCallbacks(UAnimInstance* AnimInstance);

	/** 범위 내 적대 대상 탐색 후 GE 적용 */
	void ApplyAreaDamage();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 에너미 전방으로부터의 공격 판정 거리
	UPROPERTY(EditDefaultsOnly, Category = "LR|AreaAttack")
	float StrikeDistance = 150.f;

	// 구형 범위 판정 반경
	UPROPERTY(EditDefaultsOnly, Category = "LR|AreaAttack")
	float HitRadius = 200.f;

	// 범위 공격 발동 VFX (없으면 스킵)
	UPROPERTY(EditDefaultsOnly, Category = "LR|AreaEffect")
	TSoftObjectPtr<UNiagaraSystem> AreaAttackVFX;

private:
	TObjectPtr<UAnimMontage> ActiveMontage = nullptr;
	bool bDamageApplied = false;

	static const FName HitNotifyName;
};