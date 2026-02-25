// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "UI/BaseWidget.h"

#include "LRGachaRevealWidget.generated.h"

class ALRGachaOrbSceneActor;
class UButton;
class UWidget;
class UPanelWidget;
class ULRGachaResultSlotWidget;

/**
 * ULRGachaRevealWidget (리뉴얼 버전)
 *
 * 연출 설계:
 *  - 1회:
 *      달 연출(3D 씬) → 구슬 1개 → 탭 시 실루엣/리빌 → 로비로 복귀
 *  - 10회:
 *      달 연출 → 구슬 10개 캐러셀 → 스와이프/탭으로 개별 리빌
 *      스킵 버튼: 모든 구슬 이펙트 터지며 한 번에 결과 나열 → 탭으로 로비 복귀
 *
 * 설계 포인트:
 *  - SkipState(0/1/2) 옛 로직 완전 제거
 *  - 스킵은 ButtonSkip 한 번으로만 동작
 *  - 3D 씬(ALRGachaOrbSceneActor)과 UI(이 위젯)는 완전히 분리
 *
 * C++:
 *  - OrbSceneActor 찾기/스폰
 *  - 마우스 입력(탭/스와이프) → 캐러셀 조작
 *  - 스킵 처리 및 결과 그리드 구성
 *
 * BP:
 *  - BP_OnRevealStarted: 1회/10회에 따라 UI 전환
 *  - BP_OnAllRevealed:   최종 결과 오버레이 표시(애니/사운드)
 */
UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 트랜잭션 ID 포함 리빌 시작 (리빌 맵 HUD에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults);

	// ─── 외부 진입점 (상점에서 직접 호출할 때) ──────────────────────

	/** GachaShopWidget에서 직접 호출하는 진입점 (트랜잭션 ID 없이) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartReveal(FName InBannerID, const TArray<FLRGachaResult>& InResults);

	/** 리빌 씬 종료 후 로비로 복귀 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void FinishAndClose();

protected:
	// ─── UMG 바인딩 ────────────────────────────────────────────────

	/** 스킵 버튼 - 처음부터 노출, 누르면 전체 즉시 리빌 */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UButton> ButtonSkip;

	/** “탭해서 열기” 같은 안내 텍스트 (선택 사용) */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> HintText;

	/** 결과 오버레이 루트 위젯 (BP에서 Visibility 제어) */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|Reveal", meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget> ResultOverlay;

	// ─── 결과 슬롯 컨테이너 / 슬롯 클래스 ──────────────────────────

	/** 결과 슬롯들을 담을 PanelWidget (VerticalBox, UniformGrid 등 자유) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ResultSlotContainer;

	/** C++에서 생성할 결과 슬롯 위젯 클래스 (WBP_GachaResultSlot 지정) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Result")
	TSubclassOf<ULRGachaResultSlotWidget> ResultSlotWidgetClass;

	// ─── 3D Orb Scene Actor 연동 ───────────────────────────────────

	/** 레벨에 없을 경우 C++에서 스폰할 OrbSceneActor BP 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|Scene")
	TSubclassOf<ALRGachaOrbSceneActor> OrbSceneActorClass;

	// ─── BP 이벤트 ────────────────────────────────────────────────

	/** 모든 구슬 리빌 완료(스킵 포함) 시: 결과 나열 UI 표시용 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnAllRevealed(const TArray<FLRGachaResult>& Results);

	/** 리빌 시작 시 호출 (DrawCount로 1회/10회 분기) */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnRevealStarted(int32 DrawCount);

	// ─── 입력 모드 유틸 ───────────────────────────────────────────

	/** 다음 틱에 Game+UI 모드로 입력 세팅 (마우스 커서 ON) */
	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	// ─── 마우스 입력(탭/스와이프) 처리 ─────────────────────────────

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// ─── 결과 슬롯 생성 ────────────────────────────────────────────

	/** CachedResults를 기반으로 ResultSlotContainer에 슬롯 위젯 생성 */
	void BuildResultSlots();

private:
	// ─── 스킵 버튼 핸들러 ─────────────────────────────────────────

	UFUNCTION()
	void OnClickSkip();

	// ─── OrbSceneActor 연동 ───────────────────────────────────────

	/** 레벨에서 OrbSceneActor를 찾거나, 없으면 스폰 */
	void FindOrSpawnOrbSceneActor();

	/** 구슬 하나가 탭/리빌 되었을 때 (현재는 필요 시 UI 확장용) */
	UFUNCTION()
	void HandleOrbClicked(int32 OrbIndex);

	/** 모든 구슬 리빌 완료 시 플래그 설정 */
	UFUNCTION()
	void HandleAllOrbsRevealed();

	// ─── UIManager로 자신 닫기 ─────────────────────────────────────

	void CloseSelf();

private:
	// ─── 내부 상태 ────────────────────────────────────────────────

	UPROPERTY()
	TObjectPtr<ALRGachaOrbSceneActor> OrbSceneActor;

	/** 이번 뽑기의 전체 결과 배열 */
	TArray<FLRGachaResult> CachedResults;

	/** 어떤 배너에서 뽑았는지 (Hero_FullMoon 등) */
	FName CachedBannerID;

	/** 서버 검증 / 재요청 등에 사용할 트랜잭션 ID (현재는 캐시만) */
	FGuid CachedTxnId;

	/** 모든 구슬이 리빌 완료되었는지 여부 */
	bool bAllRevealed = false;

	/** 결과 오버레이(슬롯 10칸)가 이미 생성/표시되었는지 여부 */
	bool bResultOverlayShown = false;

	// ─── 스와이프 입력 상태 ───────────────────────────────────────

	bool bIsPointerDown = false;
	FVector2D PointerDownPosition = FVector2D::ZeroVector;

	/** 스와이프로 인식할 최소 이동 거리(px) */
	float SwipeMinDistance = 30.f;

	// ─── 카메라 ViewTarget 복원용 ─────────────────────────────────

	/** 이 위젯이 카메라를 OrbSceneActor로 바꾸기 전의 ViewTarget */
	UPROPERTY()
	TObjectPtr<AActor> PreviousViewTarget = nullptr;
};