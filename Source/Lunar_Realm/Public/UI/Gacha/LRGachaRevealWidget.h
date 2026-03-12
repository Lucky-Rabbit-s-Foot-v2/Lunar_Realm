// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

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

protected:
	// ───────────────── UMG 바인딩 ─────────────────

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> ButtonSkip;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> HintText;

	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> ResultOverlay;

	/** 개별 캐릭터/장비 리빌 화면 루트 */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> PresentationOverlay;

	/** 리빌 배경 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealBackground;

	/** 실루엣 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealSilhouette;

	/** 컬러 최종 이미지 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealMain;

	/** 플래시용 흰 이미지 (없어도 동작은 가능) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RevealFlash;

	/** 이름 텍스트 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_RevealName;

	/** 결과 슬롯 컨테이너 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ResultSlotContainer;

	/** 슬롯 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	TSubclassOf<ULRGachaResultSlotWidget> ResultSlotWidgetClass;

	/** 최종 결과 슬롯이 하나씩 등장하는 간격 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	float ResultSlotAppearInterval = 0.12f;

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

	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


private:
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

	void ShowPresentation(const FLRGachaResult& Result);
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