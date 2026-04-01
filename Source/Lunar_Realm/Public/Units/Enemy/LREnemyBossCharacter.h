// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Structures/Core/LRPlayerCore.h"
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
 // (260326) KWB 보스는 DetourCrowd로 다른 에너미 안피하게 설정
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyBossCharacter : public ALREnemyCharacter
{
	GENERATED_BODY()
	
public:
	ALREnemyBossCharacter();

	void InitializeBossSpeed();

	void SetCoreAttackOverlapRadius(float InRadius);

	void RegisterMontageNotifyDelegate();

protected:
	virtual void BeginPlay() override;

	virtual void FinishDeathSequence() override;

	virtual void ApplyVisualData(const struct FEnemyStaticData& EnemyData) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "LR|Boss|Detection")
	TObjectPtr<USphereComponent> CoreAttackOverlap;

	// 몽타주 노티파이 콜백
	UFUNCTION()
	void OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	// Overlap 콜백
	UFUNCTION()
	void OnCoreOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCoreOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnBossHealthChanged(const FOnAttributeChangeData& Data);

	int32 CalculatePhase(float HealthPercent) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Boss|Phase")
	TArray<float> PhaseThresholds = { 0.50f, 0.25f };

private:
	int32 CurrentPhase = 0;

	static constexpr float SpeedPhase0 = 150.f;
	static constexpr float SpeedPhase1 = 250.f;
	static constexpr float SpeedPhase2 = 500.f;
};
