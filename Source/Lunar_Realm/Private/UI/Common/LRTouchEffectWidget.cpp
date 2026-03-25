// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRTouchEffectWidget.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/WidgetAnimation.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"

void ULRTouchEffectWidget::PlayRippleAnimation()
{
	if (Anim_TouchRipple)
	{
		PlayAnimation(Anim_TouchRipple);

		float AnimDuration = Anim_TouchRipple->GetEndTime();

		FTimerHandle HideTimer;
		GetWorld()->GetTimerManager().SetTimer(HideTimer, this, &ULRTouchEffectWidget::OnAnimationFinished, AnimDuration, false);
	}
}

void ULRTouchEffectWidget::OnAnimationFinished()
{
	RemoveFromParent();

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->ReturnTouchWidgetToPool(this);
	}
}
