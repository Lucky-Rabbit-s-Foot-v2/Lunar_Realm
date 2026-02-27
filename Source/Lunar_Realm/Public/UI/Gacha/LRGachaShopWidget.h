// LRGachaShopWidget.h
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "UI/Core/LRPageWidget.h"
#include "UI/Gacha/LRGachaRevealWidget.h"
#include "LRGachaShopWidget.generated.h"

class UButton;
class UTextBlock;
class ULRGachaSubsystem;

/**
 * ULRGachaShopWidget (가챠 상점 UI)
 *
 * 역할
 * - 탭(영웅/장비), 뽑기 버튼(초승/보름 1회/10회) 입력 처리
 * - ULRGachaSubsystem에게 트랜잭션 기반 뽑기 요청
 * - 재화/천장 텍스트 갱신(델리게이트 구독)
 * - (신 플로우) 결과를 Subsystem에 PendingReveal로 저장 후 GachaRevealMap으로 레벨 이동
 *
 * 설계
 * - “뽑기 결과 확정/저장/지급”은 Subsystem에서 처리한다.
 * - ShopWidget은 UI/이동/표시만 담당한다.
 */
UCLASS()
class LUNAR_REALM_API ULRGachaShopWidget : public ULRPageWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 현재 선택된 배너 ID */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha")
	FName CurrentBannerID;

	/** 초기 영웅 배너(BP에서 설정 가능) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultHeroBannerID = TEXT("Hero_FullMoon");

	/** 초기 장비 배너(BP에서 설정 가능) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultEquipBannerID = TEXT("Equip_FullMoon");

protected:
	// ───────────────── UMG 바인딩 ─────────────────

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ButtonHome;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonHeroTab;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipTab;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCrescentDraw1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCrescentDraw10;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonFullMoonDraw1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonFullMoonDraw10;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextPity;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextGold;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextCrescent;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextFullMoon;

private:
	// ───────────────── 서브시스템 참조 ─────────────────

	UPROPERTY()
	ULRGachaSubsystem* GachaSys = nullptr;

	// ───────────────── 내부 헬퍼 ─────────────────

	/** Draw 버튼 공통 처리: 트랜잭션 시작 → 커밋 → PendingReveal 저장 → 리빌 맵 이동 */
	void TryBeginDrawAndOpenReveal(FName BannerID, int32 Count);

	// ───────────────── 버튼 콜백 ─────────────────
	UFUNCTION()
	void OnClickHome();

	UFUNCTION()
	void OnClickHeroTab();

	UFUNCTION()
	void OnClickEquipTab();

	UFUNCTION()
	void OnClickCrescentDraw1();

	UFUNCTION()
	void OnClickCrescentDraw10();

	UFUNCTION()
	void OnClickFullMoonDraw1();

	UFUNCTION()
	void OnClickFullMoonDraw10();

	// ───────────────── 이벤트 핸들러(델리게이트) ─────────────────

	UFUNCTION()
	void HandleCurrencyChanged(ELRCurrencyType Type, int32 NewValue);

	UFUNCTION()
	void HandlePityChanged(FName BannerID, int32 NewValue);

	// ───────────────── UI 갱신 ─────────────────

	void RefreshCurrencyTexts();
	void RefreshPityText();

	// ───────────────── 배너 계산 헬퍼 ─────────────────

	FName MakeBannerIDForTicket(bool bFullMoon) const;
	bool IsHeroTabSelected() const;
};