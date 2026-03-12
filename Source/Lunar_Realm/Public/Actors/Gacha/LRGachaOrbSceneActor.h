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
	Revealing,  // 구슬 연출 중
	Revealed,   // 구슬 연출 완료(리빌 화면으로 넘어감)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllOrbsRevealed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRevealPresentationRequested, int32, OrbIndex, const FLRGachaResult&, Result);

/**
 * ALRGachaOrbSceneActor
 *
 * [역할]
 * - 리빌 맵에서 달 + 구슬 캐러셀 연출 담당
 * - 10회 뽑기 시 구슬 10개를 원형으로 배치하고 스와이프/탭 처리
 * - 구슬 리빌 완료 후 UI에 "캐릭터/장비 리빌 화면 표시" 요청
 *
 * [분리 기준]
 * - 3D 구슬/카메라/캐러셀/달 연출 : SceneActor
 * - 실루엣/컬러 복원 UI : RevealWidget
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

	/** 스와이프 입력 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void OnSwipeInput(float DeltaAngle);

	/** 현재 센터 구슬 탭 → 구슬 리빌 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void OnTapCenterOrb();

	/** 아직 안 열린 구슬들을 전부 즉시 리빌 처리 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void SkipAllReveal();

	/** 리빌 화면 종료 후 입력 다시 허용 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	void NotifyPresentationClosed();

	// ===== 상태 조회 =======================================================

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	int32 GetCenterOrbIndex() const { return CenterOrbIndex; }

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	int32 GetRevealedCount() const { return RevealedCount; }

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	bool IsAllRevealed() const { return RevealedCount >= OrbActors.Num(); }

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Scene")
	bool IsPresentationPlaying() const { return bPresentationPlaying; }

	// ===== 이벤트(위젯에서 바인딩) ==========================================

	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Scene")
	FOnAllOrbsRevealed OnAllOrbsRevealed;

	/** 개별 구슬 리빌 완료 후 UI에 리빌 화면 표시 요청 */
	UPROPERTY(BlueprintAssignable, Category = "LR|Gacha|Scene")
	FOnRevealPresentationRequested OnRevealPresentationRequested;

protected:
	// ===== Root ============================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Root")
	TObjectPtr<USceneComponent> SceneRoot;

	// ===== Orb Spawn Settings ==============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Orb|Assets")
	TSubclassOf<AActor> OrbActorClass;

	// ===== Carousel Settings ===============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Carousel|Settings")
	float CarouselRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Carousel|Settings")
	float RotationSpeed = 5.f;

	/** 탭한 구슬이 이동할 중앙 리빌 지점 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Carousel|Settings")
	FVector RevealCenterOffset = FVector(0.f, 0.f, 100.f);

	// ===== Moon Components =================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Moon")
	TObjectPtr<UStaticMeshComponent> MoonMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Moon")
	TObjectPtr<UNiagaraComponent> MoonAuraComponent;

	// ===== Moon Assets =====================================================

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UStaticMesh> MoonMesh;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UMaterialInterface> MoonMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Moon|Assets")
	TObjectPtr<UNiagaraSystem> MoonAuraSystem;

	// ===== Moon Settings ===================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	float MoonMoveDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonStartOffset = FVector(0.f, 0.f, 1200.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonEndOffset = FVector(0.f, 0.f, 500.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Moon|Settings")
	FVector MoonMeshScale = FVector(2.0f, 2.0f, 2.0f);

	// ===== BP Events =======================================================

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_PlayEnterSequence();

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_PlaySkipRevealAll(const TArray<FLRGachaResult>& Results);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Scene")
	void BP_UpdateMoonPosition(float NormalizedProgress);

	// ===== Camera ==========================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Gacha|Camera")
	TObjectPtr<UCameraComponent> CameraComp;

private:
	UFUNCTION()
	void HandleOrbRevealFinished(int32 OrbIndex, const FLRGachaResult& Result);

	// ===== Runtime State ===================================================

	TArray<FLRGachaResult> CachedResults;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> OrbActors;

	TArray<ELROrbState> OrbStates;

	int32 CenterOrbIndex = 0;
	float CurrentCarouselAngle = 0.f;
	float TargetCarouselAngle = 0.f;
	int32 RevealedCount = 0;

	float MoonMoveElapsed = 0.f;
	bool bMoonMoving = false;

	/** 개별 리빌 화면 재생 중일 때 스와이프/탭 차단 */
	bool bPresentationPlaying = false;

	// ===== Camera Breathing =================================================

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	bool bEnableCameraBreathing = true;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathAmplitudeZ = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathAmplitudeY = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathSpeed = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	bool bEnableCameraFOVBreathing = false;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Camera|Breathing")
	float CameraBreathFOVAmplitude = 1.0f;

	float CameraBreathElapsed = 0.f;
	FVector DefaultCameraBoomRelativeLocation = FVector::ZeroVector;
	float DefaultCameraFOV = 0.f;

	// ===== Internal Functions =============================================

	void SpawnOrbs();
	void RevealOrb(int32 Index);
	void SetCenterOrb(int32 NewIndex);

	void UpdateCarouselTransforms(float DeltaTime);
	void UpdateMoonSequence(float DeltaTime);
	FVector GetOrbPositionForAngle(float AngleDeg) const;

	int32 FindNextUnrevealedIndex(int32 StartIndex, int32 IndexStep, int32& OutStepCount) const;
	void UpdateCameraBreathing(float DeltaTime);
};