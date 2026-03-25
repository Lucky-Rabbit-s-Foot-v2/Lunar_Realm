// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRTouchEffectWidget.generated.h"

/**
 * (260325) BJM 제작. 터치 UI생성
 */
UCLASS()
class LUNAR_REALM_API ULRTouchEffectWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	void PlayRippleAnimation();

	void OnAnimationFinished();

protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Anim_TouchRipple;
};
