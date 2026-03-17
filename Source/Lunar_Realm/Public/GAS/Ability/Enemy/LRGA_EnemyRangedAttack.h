#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "LRGA_EnemyRangedAttack.generated.h"

class ALRProjectile;
class UGameplayEffect;

/**
 * 에너미 원거리 공격 어빌리티
 * - AIController에서 EventData.OptionalObject로 전달된 몽타주를 재생
 * - "BasicShoot" Notify 시점에 투사체 스폰
 * - 데미지는 투사체 충돌 시 LRProjectile이 처리
 */
 //=============================================================================
 // (260317) KWB 제작.
 //=============================================================================
UCLASS()
class LUNAR_REALM_API ULRGA_EnemyRangedAttack : public ULRGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULRGA_EnemyRangedAttack();

protected:
	virtual void OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<ALRProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	void UnbindMontageCallbacks(UAnimInstance* AnimInstance);

	TObjectPtr<UAnimMontage> ActiveMontage = nullptr;
	bool bProjectileSpawned = false;

	// 에디터의 AnimMontage에 설정해야 하는 Notify 이름
	static const FName ShootNotifyName;
};