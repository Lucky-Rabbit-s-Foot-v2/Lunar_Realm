// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRDamageWidget.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

void ULRDamageWidget::PlayFloatAnimation(float Damage)
{
	if (Txt_Amount)
	{
		Txt_Amount->SetText(FText::AsNumber((int32)Damage));
	}
	else
	{
		LR_SCREEN_INFO(TEXT("Txt_Amount is not bound in %s"), *GetName());
	}
	
	if (FadeInAnimation)
	{
		FWidgetAnimationDynamicEvent AnimationFinishedEvent;
		AnimationFinishedEvent.BindDynamic(this, &ULRDamageWidget::OnFadeInAnimationFinished);

		BindToAnimationFinished(FadeInAnimation, AnimationFinishedEvent);
		PlayAnimation(FadeInAnimation);
	}
	else
	{
		LR_SCREEN_INFO(TEXT("FadeInAnimation is not bound in %s"), *GetName());
		OnFadeInAnimationFinished();
	}
}

void ULRDamageWidget::OnFadeInAnimationFinished()
{
	if (FadeInAnimation)
	{
		UnbindAllFromAnimationFinished(FadeInAnimation);
	}

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->ReturnDamageWidgetToPool(this);
	}
}

void ULRDamageWidget::ActivateWidget()
{
	if (!IsInViewport())
	{
		AddToViewport(100);
	}
	SetVisibility(ESlateVisibility::Visible);
}

void ULRDamageWidget::DeactivateWidget()
{
	RemoveFromParent();
	SetVisibility(ESlateVisibility::Collapsed);
}

void ULRDamageWidget::SetDamageColor(FLinearColor InColor)
{
	if (Txt_Amount)
	{
		Txt_Amount->SetColorAndOpacity(FSlateColor(InColor));
	}
}

