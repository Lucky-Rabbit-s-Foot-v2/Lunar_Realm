// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRToolTipWidget.h"
#include "LRDamageWidget.generated.h"

/**
 * 인게임 데미지 표시용 위젯
 * - UI Manager + 오브젝트 풀링 관리를 통해 생성/제거
 */

 //=============================================================================
 // (260303) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRDamageWidget : public ULRToolTipWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void PlayFloatAnimation(float Damage);

	UFUNCTION()
	void OnFadeInAnimationFinished();

	UFUNCTION()
	void ActivateWidget();

	UFUNCTION()
	void DeactivateWidget();

	UFUNCTION()
	void SetDamageColor(FLinearColor InColor);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Amount;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FadeInAnimation;
};
