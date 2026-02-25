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
 *   - 실루엣 → 원본 머티리얼 복원 + 이미시브(구슬이 스스로 빛을 내는 정도 값) 상승 애니메이션
 *   - 등급별 색/사운드/이펙트 재생
 *   - 포커스 상태(캐러셀 중앙) 처리
 *
 * [C++ 담당]
 * - 실루엣 머티리얼 교체 / Delay 후 원본 복원
 * - Tick 기반 이미시브 애니메이션
 * - 나이아가라 이펙트 스폰 타이밍
 * - 사운드 재생 타이밍
 * - SetFocused를 통한 아우라 강도 조절
 *
 * [BP 서브클래스 담당: 에셋 & 수치 바인딩]
 * - OrbMesh / OrbMaterial / SilhouetteMaterial
 * - IdleAuraSystem / RevealBurstSystem
 * - SoundCommon ~ SoundLegendary
 * - SilhouetteDuration, EmissiveDuration, FocusedScale, DefaultScale
 * - EmissiveCommon ~ EmissiveLegendary
 *
 * [BP 이벤트]
 * - BP_OnRevealFinished : 리빌 완료 후 카메라/추가 연출
 */
UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ALRGachaOrbActor : public AActor
{
	GENERATED_BODY()

public:
	ALRGachaOrbActor();

	// AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ===== Public API ======================================================

	/** 결과 1개로 Orb를 셋업 (색/이펙트/아이들 아우라 시작) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void SetupOrb(const FLRGachaResult& InResult);

	/** 실루엣 → 리빌 애니메이션 시작 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void PlayReveal();

	/** 캐러셀에서 포커스 여부 (아우라 강도 등) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Orb")
	void SetFocused(bool bFocused);

	/** 등급 → Orb 컬러 (씬 전체에서 통일해 쓰고 싶을 때) */
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
	TObjectPtr<UAudioComponent> AudioComp;

	// ===== Assets (BP에서만 세팅) ========================================

	/** 구슬 기본 머티리얼 (OrbColor / EmissiveStrength 파라미터 필요) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> OrbMaterial;

	/** 실루엣(실루엣 연출용) 머티리얼 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UMaterialInterface> SilhouetteMaterial;

	/** 기본 아이들 아우라 나이아가라 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UNiagaraSystem> IdleAuraSystem;

	/** 리빌 순간 버스트 이펙트 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<UNiagaraSystem> RevealBurstSystem;

	/** 등급별 사운드 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundCommon;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundElite;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundUnique;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundEpic;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Assets")
	TObjectPtr<USoundBase> SoundLegendary;

	// ===== Tuning Parameters (BP에서 조정) ===============================

	/** 실루엣 유지 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float SilhouetteDuration = 0.6f;

	/** 이미시브 상승 애니메이션 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveDuration = 1.5f;

	/** 포커스 시 스케일 (SetFocused에서 사용 예정) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float FocusedScale = 1.5f;

	/** 기본 스케일 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float DefaultScale = 1.0f;

	/** 머티리얼 컬러 파라미터 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	FName ColorParamName = TEXT("OrbColor");

	/** 머티리얼 이미시브 세기 파라미터 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	FName EmissiveParamName = TEXT("EmissiveStrength");

	/** 등급별 최대 이미시브 값 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveCommon = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveElite = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveUnique = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveEpic = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Orb|Settings")
	float EmissiveLegendary = 10.0f;

	// ===== BP Events (카메라/외부 연출만) ================================

	/** 리빌 전체 연출이 끝났을 때 호출 (카메라 줌/카트씬 등 BP에서 구현) */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Orb")
	void BP_OnRevealFinished(ELRGachaRarity Rarity, const FLRGachaResult& Result);

private:
	// ===== Runtime State ===================================================

	/** 이번 Orb가 표시하는 가챠 결과 */
	FLRGachaResult CachedResult;

	/** C++ 쪽에서 생성/관리하는 Dynamic Material Instance */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMat;

	/** 이미시브 애니메이션 진행도(0~1) */
	float EmissiveAlpha = 0.f;

	/** Tick에서 이미시브 갱신 중인지 여부 */
	bool bEmissiveAnimating = false;

	/** 실루엣 유지 타이머 핸들 */
	FTimerHandle TimerSilhouette;

	// ===== Internal Functions =============================================

	/** OrbMaterial 기반으로 등급별 컬러/이미시브 세팅 */
	void ApplyMaterialParams(ELRGachaRarity Rarity);

	/** 등급 → 이미시브 값 */
	float GetEmissiveByRarity(ELRGachaRarity Rarity) const;

	/** 등급 → 사운드 에셋 */
	USoundBase* GetSoundByRarity(ELRGachaRarity Rarity) const;

	/** 실루엣 시작 처리 */
	void PlaySilhouette_Internal();

	/** SilhouetteDuration 경과 후 호출 */
	void OnSilhouetteFinished();

	/** 이미시브 애니메이션 종료 처리 */
	void OnEmissiveFinished();
};