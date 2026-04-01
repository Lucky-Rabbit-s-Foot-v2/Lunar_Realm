// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRBossAlertPopupWidget.h"

#include "Animation/WidgetAnimation.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULRBossAlertPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRBossAlertPopupWidget::NativeDestruct()
{
	OnCloseUIRequestedDel.Clear();

	Super::NativeDestruct();
}

void ULRBossAlertPopupWidget::OpenUI()
{
	Super::OpenUI();

	OnPlayAlertAnimation();
}

void ULRBossAlertPopupWidget::OnPlayAlertAnimation()
{
	if (AlertAnim)
	{
		if (IsAnimationPlaying(AlertAnim))
		{
			StopAnimation(AlertAnim);
		}
		PlayAnimation(AlertAnim);
	}
}

void ULRBossAlertPopupWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == AlertAnim)
	{
		OnCloseUIRequestedDel.Broadcast(this);
	}
}
