// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRLoadingWidget.h"

#include "Components/ProgressBar.h"
#include "TimerManager.h"

ULRLoadingWidget::ULRLoadingWidget()
{
	UILayer = EUILayer::POPUP;
	ZOrder = 1000;
}

void ULRLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Progress = 0.f;
	ElapsedTime = 0.f;

	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}

	GetWorld()->GetTimerManager().SetTimer(
		LoadingTimerHandle, 
		this, 
		&ULRLoadingWidget::UpdateProgressBar, 
		0.01f, 
		true
	);
}

void ULRLoadingWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRLoadingWidget::UpdateProgressBar()
{
	if (Bar_Loading)
	{
		float RemainingTime = TotalDuration - ElapsedTime;
		float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
		float Increment = (RandomFactor * 0.01f) / TotalDuration;

		Progress += Increment;
		ElapsedTime += 0.01f;

		if (ElapsedTime > TotalDuration)
		{
			Progress = 1.f;
			GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);
		}

		Bar_Loading->SetPercent(Progress);
	}
}
