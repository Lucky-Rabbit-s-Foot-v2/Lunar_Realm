// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRIntroWidget.h"

#include "Components/Image.h"

#include "UI/Intro/LRTitleWidget.h"

ULRIntroWidget::ULRIntroWidget()
{
	UILayer = EUILayer::PERSISTENT;
	ZOrder = 0;
}

void ULRIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
}

void ULRIntroWidget::OnFinishedIntroAnim()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			OpenTitleWidget();
		},
		0.5f,
		false
	);
}

void ULRIntroWidget::OpenTitleWidget()
{
	if (TitleWidgetClass)
	{
		UUserWidget* TitleWidget = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
		if (TitleWidget)
		{
			TitleWidget->AddToViewport();
		}
	}
}
