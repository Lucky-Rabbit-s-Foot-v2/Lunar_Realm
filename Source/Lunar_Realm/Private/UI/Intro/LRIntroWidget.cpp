// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRIntroWidget.h"

#include "Engine/GameInstance.h"
#include "TimerManager.h"

#include "Components/Image.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Intro/LRTitleWidget.h"

#include "Units/OutGame/LRIntroController.h"


void ULRIntroWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	if (ALRIntroController* IntroController = Cast<ALRIntroController>(Controller))
	{
		OnTitleOpenRequestedDel.AddUniqueDynamic(IntroController, &ALRIntroController::OpenTitleScreen);
	}
}

void ULRIntroWidget::OpenUI()
{
	Super::OpenUI();

	PlayIntroAnimation();
}

void ULRIntroWidget::RefreshUI()
{
	Super::RefreshUI();

	Img_1->SetVisibility(ESlateVisibility::Hidden);
	Img_2->SetVisibility(ESlateVisibility::Hidden);
	Img_3->SetVisibility(ESlateVisibility::Hidden);
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
			OnTitleOpenRequestedDel.Broadcast();
		},
		0.5f,
		false
	);
}
