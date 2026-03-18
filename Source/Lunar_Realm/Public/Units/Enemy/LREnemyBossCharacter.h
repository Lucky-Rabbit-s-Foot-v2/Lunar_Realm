// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "LREnemyBossCharacter.generated.h"

class ULRBossAttributeSet;

/**
 * 보스 캐릭터 클래스
 * - ULRBossAttributeSet 사용
 * - 페이즈 변경 델리게이트 수신 => BB의 CurrentPhase 키 갱신
 * - 오브젝트 풀링 미사용 (씬에 단 한 개만 존재)
 */

 //============================================================================
 // (260312) KWB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyBossCharacter : public ALREnemyCharacter
{
	GENERATED_BODY()
	
public:
	ALREnemyBossCharacter();

	void InitializeBossSpeed();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Boss|Phase")
	TArray<float> PhaseThresholds = { 0.50f, 0.20f };

private:
	void OnBossHealthChanged(const FOnAttributeChangeData& Data);
	int32 CalculatePhase(float HealthPercent) const;

	int32 CurrentPhase = 0;

	// TEST : 추후 DT에서 관리 예정
	static constexpr float SpeedPhase0 = 150.f;
	static constexpr float SpeedPhase1 = 300.f;
	static constexpr float SpeedPhase2 = 700.f;
};
