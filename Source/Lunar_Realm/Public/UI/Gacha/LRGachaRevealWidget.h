// LRGachaRevealWidget.h
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
class ULRGachaResultSlotWidget;

/**
 * ULRGachaRevealWidget (가챠 리빌 UI)
 *
 * 역할
 * - “가챠 결과를 뽑는 곳”이 아님. (결과는 Subsystem에서 이미 확정됨)
 * - 리빌 맵(GachaRevealMap)에서:
 *   1) OrbSceneActor(3D 연출 액터)를 찾거나 스폰한다.
 *   2) OrbSceneActor에 결과 배열(CachedResults)을 주입한다.
 *   3) 유저 입력(탭/스와이프)을 받아 OrbSceneActor에게 전달한다.
 *   4) 스킵/결과 오버레이/로비 복귀(샵 자동 오픈 요청)를 처리한다.
 *
 * 설계
 * - 3D 연출(OrbSceneActor)과 UI(이 위젯)를 분리한다.
 * - 스킵은 ButtonSkip “하나”로만 처리한다. (복잡한 SkipState 제거)
 * - 두 번째 리빌에서 클릭이 안 먹는 문제 방지:
 *   -> ForceUIInputNextTick()로 입력 모드를 Game+UI로 강제한다.
 *
 * BP 이벤트
 * - BP_OnRevealStarted(DrawCount):
 *     1회/10회 분기 UI / 애니메이션 시작 트리거
 * - BP_OnAllRevealed(Results):
 *     결과 오버레이 표시(최종 애니/사운드) 트리거
 */
UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	// ───────────────── UUserWidget 오버라이드 ─────────────────

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ───────────────── 외부 진입점(호출 API) ─────────────────

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

	/** 스킵 버튼(단일 버튼): 전체 리빌 → 결과 오버레이 → 로비 복귀 */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> ButtonSkip;

	/** 안내 텍스트(예: “구슬을 탭해서 열기”) */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> HintText;

	/** 결과 오버레이 루트(BP에서 Visibility/애니 제어 가능) */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> ResultOverlay;

	// ───────────────── 결과 슬롯 UI ─────────────────

	/** 결과 슬롯들을 담을 컨테이너(UniformGrid/VerticalBox 등 자유) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ResultSlotContainer;

	/** 슬롯 위젯 클래스(WBP_GachaResultSlot 지정) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	TSubclassOf<ULRGachaResultSlotWidget> ResultSlotWidgetClass;

	// ───────────────── 3D 씬 연동 ─────────────────

	/** 레벨에 없을 경우 C++에서 스폰할 OrbSceneActor BP 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Scene")
	TSubclassOf<ALRGachaOrbSceneActor> OrbSceneActorClass;

	// ───────────────── BP 이벤트 ─────────────────

	/** 모든 구슬 리빌 완료(스킵 포함) 시: 결과 나열 UI 표시용 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnAllRevealed(const TArray<FLRGachaResult>& Results);

	/** 리빌 시작 시 호출 (DrawCount로 1회/10회 분기) */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnRevealStarted(int32 DrawCount);

	// ───────────────── 입력 모드 유틸 ─────────────────

	/** 다음 틱에 Game+UI 모드로 입력 세팅 (두 번째 리빌 클릭 미작동 방지) */
	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	// ───────────────── 마우스/터치 입력 ─────────────────

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// ───────────────── 결과 슬롯 생성 ─────────────────

	/** CachedResults를 기반으로 ResultSlotContainer에 슬롯 위젯 생성 */
	void BuildResultSlots();

private:
	// ───────────────── 스킵 버튼 핸들러 ─────────────────

	UFUNCTION()
	void OnClickSkip();

	// ───────────────── OrbSceneActor 연동 ─────────────────

	/** 레벨에서 OrbSceneActor를 찾거나, 없으면 스폰 */
	void FindOrSpawnOrbSceneActor();

	/** 구슬 하나가 탭/리빌 되었을 때 (현재는 확장용 자리) */
	UFUNCTION()
	void HandleOrbClicked(int32 OrbIndex);

	/** 모든 구슬 리빌 완료 시 내부 플래그 설정 */
	UFUNCTION()
	void HandleAllOrbsRevealed();

	// ───────────────── UI 닫기 ─────────────────

	/** UIManager가 있으면 CloseUI, 없으면 RemoveFromParent 폴백 */
	void CloseSelf();

private:
	// ───────────────── 내부 상태(한 번의 리빌 단위) ─────────────────

	UPROPERTY()
	TObjectPtr<ALRGachaOrbSceneActor> OrbSceneActor;

	/** 이번 뽑기의 전체 결과 배열 */
	TArray<FLRGachaResult> CachedResults;

	/** 어떤 배너에서 뽑았는지(Hero_FullMoon 등) */
	FName CachedBannerID;

	/** 서버 검증/로그 확장용 트랜잭션 ID(현재는 캐시만) */
	FGuid CachedTxnId;

	/** 모든 구슬 리빌 완료 여부 */
	bool bAllRevealed = false;

	/** 결과 오버레이가 이미 표시되었는지 */
	bool bResultOverlayShown = false;

	// ───────────────── 스와이프 입력 상태 ─────────────────

	bool bIsPointerDown = false;
	FVector2D PointerDownPosition = FVector2D::ZeroVector;

	/** 스와이프로 인식할 최소 이동 거리(px) */
	float SwipeMinDistance = 30.f;

	// ───────────────── 카메라 복원용 ─────────────────

	/** OrbSceneActor로 ViewTarget 변경 전의 원래 ViewTarget */
	UPROPERTY()
	TObjectPtr<AActor> PreviousViewTarget = nullptr;
};