// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "Sound/SoundBase.h"

#include "UI/Core/LRBaseWidget.h"
#include "LRGachaRevealWidget.generated.h"

class ALRGachaOrbSceneActor;
class UButton;
class UWidget;
class UPanelWidget;
class UImage;
class UTextBlock;
class UMediaPlayer;
class UMediaTexture;
class ULRGachaResultSlotWidget;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UAudioComponent;

/**
 * ULRGachaRevealWidget (가챠 리빌 UI)
 *
 * 역할
 * - OrbSceneActor(3D 연출)와 UI 리빌 화면(실루엣/컬러 복원)을 연결
 * - 개별 구슬 완료 후 캐릭터/장비 리빌 화면 표시
 * - 탭/스와이프/스킵/최종 결과창 처리
 *
 * 화면 분리
 * - 3D 구슬/달/카메라 : OrbSceneActor
 * - 실루엣/컬러/배경/텍스트 : 이 위젯
 */
UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** (리빌 맵 HUD에서 호출) 트랜잭션 ID 포함 리빌 시작 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults);

	/** (상점에서 직접 호출 가능) 트랜잭션 ID 없이 리빌 시작 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartReveal(FName InBannerID, const TArray<FLRGachaResult>& InResults);

	/** 리빌 씬 종료 후 로비로 복귀 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void FinishAndClose();

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void NotifyTransitionFinished();

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void PlayCurrentColorRevealSound();

protected:
	// ───────────────── UMG 바인딩 ─────────────────

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> ButtonSkip;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Glow;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_MagicFull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Sound")
	TObjectPtr<USoundBase> ResultOverlayOpenSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Sound")
	TObjectPtr<USoundBase> RevealStageBGMSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Reveal|Sound", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float RevealStageBGMVolume = 1.0f;

	/** 문양 인트로/전환용 오버레이 */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> TransitionOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> PresentationOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> HintText;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> ResultOverlay;

	/** 리빌 배경 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealBackground;

	/** 모든 유닛 공통으로 사용할 리빌 배경 이미지 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Common")
	TSoftObjectPtr<UTexture2D> CommonRevealBackgroundTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Transition|Magic")
	TSoftObjectPtr<UTexture2D> TransitionMagicTextureN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Transition|Magic")
	TSoftObjectPtr<UTexture2D> TransitionMagicTextureR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Transition|Magic")
	TSoftObjectPtr<UTexture2D> TransitionMagicTextureSR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Transition|Magic")
	TSoftObjectPtr<UTexture2D> TransitionMagicTextureSSR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Transition|Magic")
	TSoftObjectPtr<UTexture2D> TransitionMagicTextureUR;

	/** 공통 배경을 우선 사용할지 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Common")
	bool bUseCommonRevealBackground = true;

	/** 실루엣 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealSilhouette;

	/** 컬러 최종 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealMain;

	/** 영상 출력용 이미지 (MediaTexture 머티리얼 브러시 사용) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealVideo;

	/** 플래시용 흰 이미지 (없어도 동작은 가능) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealFlash;

	/** 이름 텍스트 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_RevealName;

	/** 등급 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RarityBadge;

	/** 결과 슬롯 컨테이너 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ResultSlotContainer;

	/** 슬롯 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	TSubclassOf<ULRGachaResultSlotWidget> ResultSlotWidgetClass;

	/** 최종 결과 슬롯이 하나씩 등장하는 간격 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	float ResultSlotAppearInterval = 0.12f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Rarity")
	TSoftObjectPtr<UTexture2D> RarityTextureN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Rarity")
	TSoftObjectPtr<UTexture2D> RarityTextureR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Rarity")
	TSoftObjectPtr<UTexture2D> RarityTextureSR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Rarity")
	TSoftObjectPtr<UTexture2D> RarityTextureSSR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Rarity")
	TSoftObjectPtr<UTexture2D> RarityTextureUR;

	// ───────────────── 3D 씬 연동 ─────────────────

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Scene")
	TSubclassOf<ALRGachaOrbSceneActor> OrbSceneActorClass;

	// ───────────────── 영상 확장(선택) ─────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Video")
	TObjectPtr<UMediaPlayer> RevealMediaPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Gacha|Reveal|Video")
	TObjectPtr<UMediaTexture> RevealMediaTexture;

	// ───────────────── 애니메이션 설정 ─────────────────

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Reveal|Settings")
	float PresentationFlashDuration = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Reveal|Settings")
	float PresentationSilhouetteHold = 0.25f;

	// ───────────────── BP 이벤트 ─────────────────

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnAllRevealed(const TArray<FLRGachaResult>& Results);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnRevealStarted(int32 DrawCount);

	/** 개별 리빌 화면이 열릴 때 BP 추가 연출용 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnPresentationOpened(const FLRGachaRevealPresentationData& PresentationData);

	/** 개별 리빌 화면이 닫힐 때 BP 추가 연출용 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnPresentationClosed();

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_PlayTransitionIntro(const FLRGachaResult& Result);

	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnTransitionClosed();

	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** 모든 캐릭터 리빌 사운드 공통 딜레이(초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha|Reveal|Sound")
	float RevealSFXDelay = 0.0f;

private:
	/** 현재 재생 중인 리빌 사운드 컴포넌트 */
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> ActiveRevealSFXComponent = nullptr;

	/** 가챠 리빌 배경음 컴포넌트 */
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> RevealStageBGMComponent = nullptr;

	/** 리빌 사운드 지연 재생용 타이머 */
	FTimerHandle TimerRevealSFXDelay;

	FLinearColor GetNameColorByRarity(ELRGachaRarity Rarity) const;

	UTexture2D* GetRarityTextureByRarity(ELRGachaRarity Rarity) const;
	UTexture2D* GetTransitionMagicTextureByRarity(ELRGachaRarity Rarity) const;

	void StartRevealStageBGM();
	void StopRevealStageBGM();

	void StopActiveRevealSFX();
	void StopAllResultSlotSounds();
	void StopAllPreviousStageSounds();

	void ApplyTransitionTexturesByRarity(ELRGachaRarity Rarity);
	void ResetTransitionVisuals();
	void ShowFinalResultOverlay();

	/** MediaTexture를 UI에 표시할 머티리얼 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Reveal|Video")
	TObjectPtr<UMaterialInterface> RevealVideoMaterial;

	/** 런타임 생성 영상 머티리얼 인스턴스 */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RevealVideoMID;

	/** 전환 애니메이션이 끝난 뒤 표시할 대기 결과 */
	FLRGachaResult PendingTransitionResult;

	/** 전환 애니메이션이 끝난 뒤 표시할 대기 인덱스 */
	int32 PendingTransitionOrbIndex = INDEX_NONE;

	/** 전환 애니메이션 진행 중 여부 */
	bool bTransitionPlaying = false;

	/** 현재 프레젠테이션이 영상 모드인지 */
	bool bPresentationUsingVideo = false;

	/** 최종 결과 슬롯을 한 번에 즉시 생성하는 기본 함수 */
	void BuildResultSlots();

	/** 최종 결과 슬롯을 일정 간격으로 하나씩 생성하는 순차 연출용 함수 */
	void BuildResultSlotsSequential();

	/** 순차 생성 중 다음 결과 슬롯 1개를 실제로 생성하는 함수 */
	void SpawnNextResultSlot();

	// ───────────────── 스킵 버튼 핸들러 ─────────────────

	UFUNCTION()
	void OnClickSkip();

	// ───────────────── OrbSceneActor 연동 ─────────────────

	void FindOrSpawnOrbSceneActor();

	UFUNCTION()
	void HandleAllOrbsRevealed();

	UFUNCTION()
	void HandleRevealPresentationRequested(int32 OrbIndex, const FLRGachaResult& Result);

	// ───────────────── 개별 리빌 화면 제어 ─────────────────

	void ShowPresentation(int32 OrbIndex, const FLRGachaResult& Result);
	void HidePresentation();

	/** PresentationData를 UMG에 반영 */
	void ApplyPresentationDataToWidgets(const FLRGachaRevealPresentationData& InData);

	// ───────────────── UI 닫기 ─────────────────

	void CloseSelf();

private:
	// ───────────────── 내부 상태(한 번의 리빌 단위) ─────────────────

	UPROPERTY()
	TObjectPtr<ALRGachaOrbSceneActor> OrbSceneActor;

	TArray<FLRGachaResult> CachedResults;
	FName CachedBannerID;
	FGuid CachedTxnId;

	bool bAllRevealed = false;
	bool bResultOverlayShown = false;

	/** 개별 캐릭터/장비 리빌 화면 표시 중 여부 */
	bool bPresentationVisible = false;

	/** 현재 화면에 표시 중인 결과 */
	FLRGachaResult CurrentPresentationResult;

	/** 현재 화면 표시용 데이터 */
	FLRGachaRevealPresentationData CurrentPresentationData;

	void StartTransitionSequence(int32 OrbIndex, const FLRGachaResult& Result);

	// ───────────────── 스와이프 입력 상태 ─────────────────

	bool bIsPointerDown = false;
	FVector2D PointerDownPosition = FVector2D::ZeroVector;
	float SwipeMinDistance = 30.f;

	/** 최종 결과 슬롯 순차 생성을 반복 호출하는 타이머 */
	FTimerHandle TimerSequentialResultSlots;

	/** 다음에 생성할 결과 슬롯 인덱스 */
	int32 NextResultSlotIndex = 0;

	/** 순차 등장 연출에 사용하는 결과 슬롯 위젯 캐시 */
	UPROPERTY()
	TArray<TObjectPtr<ULRGachaResultSlotWidget>> CachedResultSlotWidgets;

	// ───────────────── 카메라 복원용 ─────────────────

	UPROPERTY()
	TObjectPtr<AActor> PreviousViewTarget = nullptr;
};