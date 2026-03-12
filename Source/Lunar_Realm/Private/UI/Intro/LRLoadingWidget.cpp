// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRLoadingWidget.h"

#include "Components/ProgressBar.h"

#include "TimerManager.h"

#include "Components/Throbber.h"
#include "Components/Image.h"

void ULRLoadingWidget::OpenUI()
{
	Super::OpenUI();
	
	GetWorld()->GetTimerManager().SetTimer(
		LoadingTimerHandle,
		this,
		&ULRLoadingWidget::UpdateLoadingAnimation,
		TimerInterval,
		true
	);
}

void ULRLoadingWidget::CloseUI()
{
	Super::CloseUI();

	GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);
}

void ULRLoadingWidget::RefreshUI()
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

void ULRLoadingWidget::UpdateLoadingAnimation()
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



	if (Bar_Loading)
	{
		float RemainingTime = TotalDuration - ElapsedTime;
		float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
		float Increment = (RandomFactor * 0.01f) / TotalDuration;

		Progress += Increment;
		ElapsedTime += 0.01f;

		if (ElapsedTime >= TotalDuration)
		{
			FinishLoading();
		}

		Bar_Loading->SetPercent(Progress);
	}
}

void ULRLoadingWidget::FinishLoading()
{
	Progress = 1.f;
	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}
}
