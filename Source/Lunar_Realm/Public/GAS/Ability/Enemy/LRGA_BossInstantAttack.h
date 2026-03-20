#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Enemy/LRGA_InstantAttack.h"
#include "LRGA_BossInstantAttack.generated.h"

/**
 * 보스 전용 근접 공격 GA
 * - 추후 사용 예정
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
};