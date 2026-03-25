// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRLoadingPageWidget.h"

#include "Components/ProgressBar.h"

#include "TimerManager.h"

#include "Components/Throbber.h"
#include "Components/Image.h"

void ULRLoadingPageWidget::OpenUI()
{
	Super::OpenUI();
	
	GetWorld()->GetTimerManager().SetTimer(
		LoadingTimerHandle,
		this,
		&ULRLoadingPageWidget::UpdateLoadingAnimation,
		TimerInterval,
		true
	);
}

void ULRLoadingPageWidget::CloseUI()
{
	Super::CloseUI();

	GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);
}

void ULRLoadingPageWidget::RefreshUI()
{
	Super::RefreshUI();
	
	CurrentRotation = 0.f;
	AnimationTime = 0.f;

	ElapsedTime = 0.f;
	Progress = 0.f;
	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}
	GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);
}

void ULRLoadingPageWidget::UpdateLoadingAnimation()
{
	AnimationTime += TimerInterval;

	if (Img_Gear)
	{
		CurrentRotation += RotationPerSecond * TimerInterval; 
		if (CurrentRotation >= 360.f) 
		{
			CurrentRotation -= 360.f;
		}

		Img_Gear->SetRenderTransformAngle(CurrentRotation);
	}

	if (Img_Icon)
	{
		float CurrentWobbleAngle = FMath::Sin(AnimationTime * WobbleFrequency) * WobbleAngle;
		Img_Icon->SetRenderTransformAngle(CurrentWobbleAngle);
	}
}

void ULRLoadingPageWidget::SetLoadingProgress(float InProgress)
{
	Progress = FMath::Clamp(InProgress, 0.0f, 1.0f);
	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}
}

void ULRLoadingPageWidget::FinishLoading()
{
	SetLoadingProgress(1.0f);
}
