#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Enemy/LRGA_InstantAttack.h"
#include "LRGA_BossInstantAttack.generated.h"

/**
 * 보스 전용 근접 공격 GA
 * - LRGA_InstantAttack을 상속
 * - GE Spec에 Attack.Boss.Melee 태그를 추가하여
 *   플레이어 코어에 닿으면 즉시 파괴되도록 신호를 보냄
 */
 // ============================================================================
 // (260319) KWB 제작.
 // ============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_BossInstantAttack : public ULRGA_InstantAttack
{
	GENERATED_BODY()

public:
	ULRGA_BossInstantAttack();

protected:
	virtual void ModifyDamageSpec(FGameplayEffectSpecHandle& InSpecHandle) override;
};