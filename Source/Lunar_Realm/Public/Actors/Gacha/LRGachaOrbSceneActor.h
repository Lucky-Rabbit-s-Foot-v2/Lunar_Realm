// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"

#include "Data/LRDataStructs.h"

#include "Components/StaticMeshComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Materials/MaterialInterface.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMesh.h"

#include "LRGachaOrbSceneActor.generated.h"

class UNiagaraSystem;
class ALRGachaOrbActor;

/**
 * 구슬 하나의 상태
 */
UENUM(BlueprintType)
enum class ELROrbState : uint8
{
	Idle,       // 기본(아우라만 흐름)
	Focused,    // 캐러셀 중앙, 포커스 상태
	Revealing,  // 리빌 연출 중
	Revealed,   // 리빌 완료
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllOrbsRevealed);

/**
 * ALRGachaOrbSceneActor
 *
 * [역할]
 * - 리빌 맵에서 달 + 구슬 캐러셀 연출 담당
 * - 10회 뽑기 시 구슬 10개를 원형으로 배치하고 스와이프/탭 처리
 *
 * [C++ 담당]
 * - 구슬 캐러셀(원형 배치, 회전, 포커스 인덱스)
 * - 달 이동 타임라인
 * - 달 메쉬/나이아가라 에셋 적용
 * - 스와이프 입력 처리 (다음 미리빌 Orb 탐색)
 * - 개별 Orb 리빌/스킵 상태 관리
 *
 * [BP 서브클래스 담당]
 * - OrbActorClass : 스폰할 BP Orb 클래스
 * - 달 관련 에셋(MoonMesh, MoonMaterial, MoonAuraSystem)
 * - 튜닝 값 (CarouselRadius, RotationSpeed, CenterOrbScale, MoonMoveDuration 등)
 *
 * [BP 이벤트]
 * - BP_PlayEnterSequence : 씬 진입 시 카메라 연출
 * - BP_PlaySkipRevealAll : SkipAllReveal 후 카메라 연출
 * - BP_UpdateMoonPosition : 달 이동 중 추가 연출 (선택)
 */
UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ALRGachaOrbSceneActor : public AActor
{
	GENERATED_BODY()

public:
	ALRGachaOrbSceneActor();

	// AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ===== 외부 API ========================================================

	/** 가챠 결과들을 넘겨주면 구슬 및 달 연출 초기화 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void InitializeWithResults(const TArray<FLRGachaResult>& InResults);

	/** 스와이프 입력 (DeltaAngle > 0 / < 0 방향에 따라 다음 구슬 탐색) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void OnSwipeInput(float DeltaAngle);

	/** 현재 센터 구슬 탭 → 리빌 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void OnTapCenterOrb();

	/** 아직 안 열린 구슬들을 전부 즉시 리빌 처리 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void SkipAllReveal();

	// ===== 상태 조회 =======================================================

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	int32 GetCenterOrbIndex() const { return CenterOrbIndex; }

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	int32 GetRevealedCount() const { return RevealedCount; }

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	bool IsAllRevealed() const { return RevealedCount >= OrbActors.Num(); }

	// ===== 이벤트(위젯에서 바인딩) ==========================================

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Scene")
	FOnAllOrbsRevealed OnAllOrbsRevealed;

protected:
	// ===== Root ============================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Root")
	TObjectPtr<USceneComponent> SceneRoot;

	// ===== Orb Spawn Settings ==============================================

	/** 스폰할 구슬 BP 클래스 (ALRGachaOrbActor를 상속한 BP 권장) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Orb|Assets")
	TSubclassOf<AActor> OrbActorClass;

	// ===== Carousel Settings ===============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Carousel|Settings")
	float CarouselRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Carousel|Settings")
	float RotationSpeed = 5.f;

	// ===== Moon Components =================================================

	/** 달 스태틱 메쉬 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Moon")
	TObjectPtr<UStaticMeshComponent> MoonMeshComp;

	/** 달 아우라 나이아가라 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Moon")
	TObjectPtr<UNiagaraComponent> MoonAuraComponent;

	// ===== Moon Assets (BP에서 세팅) ======================================

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UStaticMesh> MoonMesh;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UMaterialInterface> MoonMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UNiagaraSystem> MoonAuraSystem;

	// ===== Moon Settings (BP에서 튜닝) ====================================

	/** 달 이동 시간(초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	float MoonMoveDuration = 2.5f;

	/** 달 시작 위치 오프셋 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonStartOffset = FVector(0.f, 0.f, 1200.f);

	/** 달 최종 위치 오프셋 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonEndOffset = FVector(0.f, 0.f, 500.f);

	/** 달 메쉬 스케일 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonMeshScale = FVector(2.0f, 2.0f, 2.0f);

	// ===== BP Events (카메라/달 연출) =====================================

	/** 씬 진입 카메라 연출 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_PlayEnterSequence();

	/** SkipAllReveal 이후 카메라 연출 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_PlaySkipRevealAll(const TArray<FLRGachaResult>& Results);

	/** 달 이동 중, 0~1 진행도 기반 추가 연출 (선택사항) */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_UpdateMoonPosition(float NormalizedProgress);

	// ===== Camera ==========================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Camera")
	TObjectPtr<UCameraComponent> CameraComp;

private:
	UFUNCTION()
	void HandleOrbRevealFinished(int32 OrbIndex);

	// ===== Runtime State ===================================================

	/** 가챠 결과 캐시 (InitializeWithResults에서 세팅) */
	TArray<FLRGachaResult> CachedResults;

	/** 스폰된 Orb 액터들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OrbActors;

	/** 각 Orb의 상태 배열 */
	TArray<ELROrbState> OrbStates;

	/** 현재 캐러셀 중앙에 오는 Orb 인덱스 */
	int32 CenterOrbIndex = 0;

	/** 현재 캐러셀 회전 각도 */
	float CurrentCarouselAngle = 0.f;

	/** 목표 캐러셀 회전 각도 (Tick에서 보간) */
	float TargetCarouselAngle = 0.f;

	/** 이미 리빌된 Orb 개수 */
	int32 RevealedCount = 0;

	/** 달 이동 경과 시간 */
	float MoonMoveElapsed = 0.f;

	/** 달 이동 중인지 여부 */
	bool bMoonMoving = false;

	// ===== Camera Breathing ==================================================

/** 카메라 미세 호흡 연출 활성화 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	bool bEnableCameraBreathing = true;

	/** 위아래 호흡 크기 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathAmplitudeZ = 6.f;

	/** 좌우/앞뒤 미세 흔들림 크기 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathAmplitudeY = 2.f;

	/** 호흡 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathSpeed = 0.35f;

	/** FOV 미세 변화 사용 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	bool bEnableCameraFOVBreathing = false;

	/** FOV 변화 크기 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathFOVAmplitude = 1.0f;

	/** 호흡 시작 후 누적 시간 */
	float CameraBreathElapsed = 0.f;

	/** CameraBoom 기본 위치 */
	FVector DefaultCameraBoomRelativeLocation = FVector::ZeroVector;

	/** Camera 기본 FOV */
	float DefaultCameraFOV = 0.f;

	// ===== Internal Functions =============================================

	/** OrbActors/OrbStates 초기화 및 스폰 */
	void SpawnOrbs();

	/** 특정 인덱스의 구슬 리빌 */
	void RevealOrb(int32 Index);

	/** 캐러셀 중앙 Orb 인덱스 변경 및 포커스 상태 갱신 */
	void SetCenterOrb(int32 NewIndex);

	/** 캐러셀 회전/위치 갱신(Tick) */
	void UpdateCarouselTransforms(float DeltaTime);

	/** 달 이동(Tick) */
	void UpdateMoonSequence(float DeltaTime);

	/** 각도 기반 Orb 위치 계산 */
	FVector GetOrbPositionForAngle(float AngleDeg) const;

	/**
	 * 스와이프 방향으로 다음 "미리빌" 구슬 인덱스 + 이동 칸 수 찾기
	 * @param StartIndex  시작 인덱스(현재 센터)
	 * @param IndexStep   +1 또는 -1
	 * @param OutStepCount 실제 이동하게 될 칸 수 (못 찾으면 0)
	 */
	int32 FindNextUnrevealedIndex(int32 StartIndex, int32 IndexStep, int32& OutStepCount) const;

	/** 카메라 미세 호흡(Tick) */
	void UpdateCameraBreathing(float DeltaTime);
};