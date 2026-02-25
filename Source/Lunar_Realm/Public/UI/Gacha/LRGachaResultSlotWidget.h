// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/BaseWidget.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"

#include "LRGachaResultSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

/**
 * 가챠 결과 한 칸을 표시하는 슬롯 위젯.
 *
 * 표시 요소:
 * - 아이콘
 * - 이름
 * - 등급 프레임 색
 * - NEW 뱃지 (BP 확장 여지)
 * - 중복 시 골드 전환 정보 (BP 확장 여지)
 *
 * C++:
 *  - SetupWithResult로 결과 세팅
 *  - 등급 → 색상 매핑
 *
 * BP:
 *  - BindWidget로 실제 UMG 슬롯에 연결
 *  - NEW 아이콘, 애니메이션 등 연출 추가
 */
UCLASS(BlueprintType, Blueprintable)
class LUNAR_REALM_API ULRGachaResultSlotWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	/** C++/BP 어디서든 결과 1개 넣어서 슬롯 세팅 */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|ResultSlot")
	void SetupWithResult(const FLRGachaResult& InResult);

	/** 현재 슬롯이 보여주는 결과 (BP에서 읽어 쓰기용) */
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|ResultSlot")
	const FLRGachaResult& GetResult() const { return CachedResult; }

protected:
	// ─── BindWidget 영역 (UMG에서 이름 맞춰야 함) ─────────────────────

	/** 아이콘 이미지 (캐릭터/장비 아이콘) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Icon;

	/** 등급 프레임(또는 외곽선) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_RarityFrame;

	/** 아이템 이름 텍스트 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	/** 전체 슬롯 배경(등급별 색/그라데이션 넣고 싶을 때) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border_Background;

	// ─── 내부 데이터 ────────────────────────────────────────────────

	/** 이 슬롯이 표현하는 뽑기 결과 */
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha|ResultSlot", meta = (AllowPrivateAccess = "true"))
	FLRGachaResult CachedResult;

	// ─── 헬퍼 함수 ─────────────────────────────────────────────────

	/** 등급 → 색 매핑 (OrbActor와 색을 맞춰주면 좋음) */
	FLinearColor GetColorByRarity(ELRGachaRarity Rarity) const;

	/** 이름/아이콘 세팅용 헬퍼 – 나중에 GameDataSubsystem 연동 예정 */
	void SetupNameAndIcon();
};