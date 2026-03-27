// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRReadyStartWidget.generated.h"

/**
 * Ready ~ Start 연출 위젯
 * - 인게임 진입 시 화면 전체를 덮고 입력을 차단
 * - Ready → Start 이미지 전환 애니메이션 재생
 * - 애니메이션 종료 시 OnReadySequenceFinished 델리게이트 브로드캐스트
 */
 //=============================================================================
 // (260327) KWB 제작.
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReadySequenceFinished);

UCLASS()
class LUNAR_REALM_API ULRReadyStartWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	// 연출 종료 시 브로드캐스트 — GameMode, SoundManager 등 외부에서 바인딩
	UPROPERTY(BlueprintAssignable, Category = "LR|Ready")
	FOnReadySequenceFinished OnReadySequenceFinished;

	virtual void OpenUI() override;

	virtual void NativeDestruct() override;

protected:
	// 애니메이션 종료 콜백
	UFUNCTION()
	void OnSequenceAnimFinished();

	// 위젯 정리 (입력 해제 + 뷰포트에서 제거)
	void CleanupWidget();

protected:
	// UMG 애니메이션 — 위젯 블루프린트에서 바인딩
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AnimReadyStart;
};
