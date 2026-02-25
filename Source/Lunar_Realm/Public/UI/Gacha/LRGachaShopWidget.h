// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "UI/Core/LRBaseWidget.h"
#include "UI/Gacha/LRGachaRevealWidget.h"

#include "LRGachaShopWidget.generated.h"

class UButton;
class UTextBlock;
class ULRGachaSubsystem;

/**
 * 가챠 상점 UI
 *
 * 역할:
 * - 탭/뽑기 버튼 처리
 * - LRGachaSubsystem에게 트랜잭션 요청
 * - 재화/천장 텍스트 갱신
 * - (신버전) GachaRevealMap으로 레벨 전환 + PendingReveal 캐시
 */
UCLASS()
class LUNAR_REALM_API ULRGachaShopWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	// UUserWidget overrides
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 현재 선택된 배너 ID (FullMoon 기준 Hero_FullMoon / Equip_FullMoon 등) */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha")
	FName CurrentBannerID;

	/** 초기 영웅 배너 (BP에서 설정) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultHeroBannerID = TEXT("Hero_FullMoon");

	/** 초기 장비 배너 (BP에서 설정) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultEquipBannerID = TEXT("Equip_FullMoon");

protected:
	// ───────────────── UI 위젯 바인딩 ─────────────────

	// 탭 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonHeroTab;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonEquipTab;

	// 초승달/보름달 1회/10회
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCrescentDraw1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCrescentDraw10;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonFullMoonDraw1;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonFullMoonDraw10;

	// 천장 표시 텍스트 (보름달 배너 기준)
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextPity;

	// 재화 표시 텍스트
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextGold;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextCrescent;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextFullMoon;

private:
	// ───────────────── 내부 상태/설정 ─────────────────

	/** 가챠 서브시스템(트랜잭션/재화/천장 관리) */
	UPROPERTY()
	ULRGachaSubsystem* GachaSys = nullptr;

	/** Draw 버튼 클릭 시: 트랜잭션 시작 + 리빌 맵 이동 */
	void TryBeginDrawAndOpenReveal(FName BannerID, int32 Count);

	// ───────────────── 버튼 콜백 ─────────────────

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

	// ───────────────── 이벤트 핸들러 ─────────────────

	UFUNCTION()
	void HandleCurrencyChanged(ELRCurrencyType Type, int32 NewValue);

	UFUNCTION()
	void HandlePityChanged(FName BannerID, int32 NewValue);

	// ───────────────── UI 갱신 헬퍼 ─────────────────

	/** 상단 재화 텍스트 전체 갱신 */
	void RefreshCurrencyTexts();

	/** 현재 탭 기준 천장 텍스트 갱신 */
	void RefreshPityText();

	/** 현재 탭/티켓 종류에 맞는 배너 ID 계산 */
	FName MakeBannerIDForTicket(bool bFullMoon) const;

	/** 현재 탭이 영웅 탭인지 여부 */
	bool IsHeroTabSelected() const;
};