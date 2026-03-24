#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Enemy/LRGA_EnemyRangedAttack.h"
#include "LRGA_EnemyArcAttack.generated.h"

/**
 * 에너미 곡사형 투사체 공격 GA
 * - LRGA_EnemyRangedAttack과 동한 로직
 * - 트리거 태그, 쿨다운 태그, DT 참조 키만 상이
 * - BP에서 ProjectileClass를 LRArcProjectile로 세팅
 */
 // ============================================================================
 // (260323) KWB 제작.
 // ============================================================================

UCLASS()
class LUNAR_REALM_API ULRGA_EnemyArcAttack : public ULRGA_EnemyRangedAttack
{
	GENERATED_BODY()

public:
	ULRGA_EnemyArcAttack();
};