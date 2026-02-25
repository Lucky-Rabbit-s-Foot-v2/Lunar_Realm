// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseHUD.h"
#include "LRGachaRevealHUD.generated.h"

class ULRGachaRevealWidget;

/**
 * 가챠 리빌 전용 HUD.
 *
 * 역할:
 *  - 리빌 맵 진입 시, GachaSubsystem에서 PendingReveal 소비
 *  - UIManagerSubsystem을 통해 리빌 위젯 오픈
 *  - 이상 상태(서브시스템 null, Pending 없음 등) 시 로비로 되돌리기
 */
UCLASS()
class LUNAR_REALM_API ALRGachaRevealHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	/** 레벨 시작 시 리빌 처리 진입점 */
	virtual void BeginPlay() override;

protected:
	/** 가챠 리빌에 사용할 위젯 클래스 (BP_GachaRevealWidget 지정) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|Gacha|RevealHUD")
	TSubclassOf<ULRGachaRevealWidget> RevealWidgetClass;
};