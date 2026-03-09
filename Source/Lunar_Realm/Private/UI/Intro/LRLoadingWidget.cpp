// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRLoadingWidget.h"

#include "Components/ProgressBar.h"

#include "TimerManager.h"

void ULRLoadingWidget::OpenUI()
{
	Super::OpenUI();
	
	GetWorld()->GetTimerManager().SetTimer(
		LoadingTimerHandle,
		this,
		&ULRLoadingWidget::UpdateProgressBar,
		0.01f,
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
	
	ElapsedTime = 0.f;
	Progress = 0.f;
	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}
	GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);
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

		if (ElapsedTime >= TotalDuration)
		{
			FinishLoading();
		}

		Bar_Loading->SetPercent(Progress);
	}
}

void ULRLoadingWidget::FinishLoading()
{
	GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);

	Progress = 1.f;
	if (Bar_Loading)
	{
		Bar_Loading->SetPercent(Progress);
	}
}
