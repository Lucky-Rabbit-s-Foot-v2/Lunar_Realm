// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRIntroWidget.h"

#include "TimerManager.h"
#include "Components/Image.h"

void ULRIntroWidget::NativeDestruct()
{
	OnIntroAnimFinishedDel.Clear();

	Super::NativeDestruct();
}

void ULRIntroWidget::OpenUI()
{
	Super::OpenUI();

	RefreshUI();

	PlayIntroAnimation();
}

void ULRIntroWidget::RefreshUI()
{
	Super::RefreshUI();

	Img_1->SetVisibility(ESlateVisibility::Hidden);
	Img_2->SetVisibility(ESlateVisibility::Hidden);
	Img_3->SetVisibility(ESlateVisibility::Hidden);

	Img_1->SetIsEnabled(false);
	Img_2->SetIsEnabled(false);
	Img_3->SetIsEnabled(false);
}

void ULRIntroWidget::PlayIntroAnimation()
{
	if (FadeAnim)
	{
		FWidgetAnimationDynamicEvent AnimFinishedDel;
		AnimFinishedDel.BindDynamic(this, &ULRIntroWidget::OnFinishedIntroAnim);
	
		BindToAnimationEvent(FadeAnim, AnimFinishedDel, EWidgetAnimationEvent::Finished);

		PlayAnimation(FadeAnim);
	}
	else
	{
		LR_INFO(TEXT("FadeAnim is nullptr in ULRIntroWidget::PlayIntroAnimation"));
		OnFinishedIntroAnim();
	}
}

void ULRIntroWidget::OnFinishedIntroAnim()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			OnIntroAnimFinishedDel.Broadcast();
		},
		0.5f,
		false
	);
}
