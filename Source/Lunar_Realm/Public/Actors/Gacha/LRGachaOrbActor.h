// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Sound/SoundBase.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "LRGachaOrbActor.generated.h"

/**
 * ALRGachaOrbActor
 *
 * [역할]
 * - 1개의 가챠 구슬(Orb)에 대한 연출 담당 액터
 * - 구슬 자체 연출만 담당하며, 캐릭터/장비 리빌 화면은 담당하지 않음
 *
 * [현재 리빌 플로우]
 * - SetupOrb : 결과 1개를 받아 구슬 등급색/Idle Aura 초기화
 * - PlayRevealToCenter :
 *   1) 화면 중앙(목표 위치)으로 이동
 *   2) 이미시브 상승
 *   3) 버스트/사운드 재생
 *   4) 구슬 숨김
 *   5) SceneActor에게 "이제 리빌 화면 표시 가능" 신호 전달
 *
 * [구조 분리 이유]
 * - 구슬은 실루엣 효과를 사용하지 않음
 * - 캐릭터/장비 실루엣 → 컬러 복원은 UMG 리빌 화면에서 처리
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOrbRevealFinished, int32, OrbIndex, const FLRGachaResult&, Result);

UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ALRGachaOrbActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void StopAllOrbSounds();

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void CancelRevealAndStopAllEffects();

	ALRGachaOrbActor();

	/** (SceneActor가 바인딩) 이 Orb의 인덱스 세팅 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void SetOrbIndex(int32 InIndex) { OrbIndex = InIndex; }

	/** 이 Orb 인덱스 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	int32 GetOrbIndex() const { return OrbIndex; }

	/** 리빌 연출 완료 시 SceneActor로 알림 */
	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Orb")
	FOnOrbRevealFinished OnOrbRevealFinished;

	// AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ===== Public API ======================================================

	/** 결과 1개로 Orb를 셋업 (색/이펙트/아이들 아우라 시작) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void SetupOrb(const FLRGachaResult& InResult);

	/** 구슬 리빌 시작 : 중앙 이동 → 발광 → 숨김 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void PlayRevealToCenter(const FVector& InTargetWorldLocation);

	/** 캐러셀에서 포커스 여부 (아우라 강도 등) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void SetFocused(bool bFocused);

	/** 등급 → Orb 컬러 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	static FLinearColor GetOrbColorByRarity(ELRGachaRarity Rarity);

	/** 현재 Orb가 가진 결과 데이터 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	const FLRGachaResult& GetResult() const { return CachedResult; }

protected:
	// ===== Components ======================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Orb")
	TObjectPtr<UStaticMeshComponent> OrbMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Orb")
	TObjectPtr<UNiagaraComponent> IdleAura;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Orb")
	TObjectPtr<UNiagaraComponent> IdleAuraSecondary;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Orb")
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> ActiveStartSoundComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> ActiveMainSoundComponent = nullptr;

	// ===== Assets (BP에서만 세팅) ========================================

	/** 기본 fallback 머티리얼 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialDefault;

	/** 등급별 구슬 머티리얼 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialN;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialSSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterialUR;

	/** 기본 아이들 아우라 나이아가라 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UNiagaraSystem> IdleAuraSystem;

	/** 추가 아이들 아우라 나이아가라 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UNiagaraSystem> IdleAuraSystemSecondary;

	/** 리빌 순간 버스트 이펙트 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UNiagaraSystem> RevealBurstSystem;

	/** 등급별 사운드 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundN;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundSSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundUR;

	/** 구슬 클릭 직후(중앙 이동 시작 시) 재생할 등급별 시작 사운드 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> StartSoundN;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> StartSoundR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> StartSoundSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> StartSoundSSR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> StartSoundUR;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings|Sound")
	float StartSoundDelay = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings|Sound")
	float SoundDelay = 0.08f;

	// ===== Tuning Parameters (BP에서 조정) ===============================

	/** 중앙으로 이동하는 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float MoveToCenterDuration = 0.45f;

	/** 이미시브 상승 애니메이션 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveDuration = 0.4f;

	/** 중앙 도착 후 유지 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float HoldAtCenterDuration = 0.15f;

	/** 머티리얼 컬러 파라미터 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	FName ColorParamName = TEXT("OrbColor");

	/** 머티리얼 이미시브 세기 파라미터 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	FName EmissiveParamName = TEXT("EmissiveStrength");

	/** 등급별 최대 이미시브 값 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveN = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveR = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveSR = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveSSR = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveUR = 10.0f;

	/** 중앙 이동 중 약간 확대할 배율 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float RevealScaleMultiplier = 1.25f;

	// ===== BP Events ======================================================

	/** 중앙 이동 시작 시 BP 추가 연출용 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Orb")
	void BP_OnRevealStarted(const FLRGachaResult& Result);

private:
	/** 등급 → 시작 사운드 에셋 */
	USoundBase* GetStartSoundByRarity(ELRGachaRarity Rarity) const;

	/** 캐러셀에서 이 Orb가 담당하는 인덱스 */
	UPROPERTY()
	int32 OrbIndex = INDEX_NONE;

	// ===== Runtime State ===================================================

	/** 이번 Orb가 표시하는 가챠 결과 */
	FLRGachaResult CachedResult;

	/** C++ 쪽에서 생성/관리하는 Dynamic Material Instance */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMat;

	/** 초기 위치/스케일 (리빌 시작 전 상태 복원용) */
	FVector CachedStartLocation = FVector::ZeroVector;
	FVector CachedStartScale = FVector(1.f);

	/** 중앙 목표 위치 */
	FVector RevealTargetLocation = FVector::ZeroVector;

	/** 이미시브 애니메이션 진행도(0~1) */
	float EmissiveAlpha = 0.f;

	/** 중앙 이동 진행도(0~1) */
	float MoveAlpha = 0.f;

	/** 상태 플래그 */
	bool bRevealMoving = false;
	bool bEmissiveAnimating = false;
	bool bRevealFinished = false;

	/** 중앙 유지 후 종료용 타이머 */
	FTimerHandle TimerFinishReveal;

	/** 시작 사운드 지연 재생용 타이머 */
	FTimerHandle TimerStartSoundDelay;

	/** 중앙 도착 사운드 지연 재생용 타이머 */
	FTimerHandle TimerMainSoundDelay;

	// ===== Internal Functions =============================================

	/** OrbMaterial 기반으로 등급별 컬러/이미시브 세팅 */
	void ApplyMaterialParams(ELRGachaRarity Rarity);

	/** 등급 → 이미시브 값 */
	float GetEmissiveByRarity(ELRGachaRarity Rarity) const;

	/** 등급 → 사운드 에셋 */
	USoundBase* GetSoundByRarity(ELRGachaRarity Rarity) const;

	/** 중앙 이동 종료 처리 */
	void OnMoveToCenterFinished();

	/** 이미시브 애니메이션 종료 처리 */
	void OnEmissiveFinished();

	/** 실제 리빌 완료 처리 */
	void FinishReveal();

	/** 등급별 머티리얼 선택 */
	UMaterialInterface* GetMaterialByRarity(ELRGachaRarity Rarity) const;
};