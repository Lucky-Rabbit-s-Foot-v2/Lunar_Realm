// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Transition/LRTransitionGameMode.h"

#include "UI/Intro/LRLoadingWidget.h"
#include "Core/LRGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ALRTransitionGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULRLoadingWidget>(GetWorld(), LoadingWidgetClass);
		LoadingWidgetInstance->AddToViewport(1000);
	}

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance()))
	{
		TargetLevelName = GI->GetNextLevelName();

		if (!TargetLevelName.IsNone())
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.ExecutionFunction = FName("OnLevelPreloaded");
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = FMath::Rand();

			UGameplayStatics::LoadStreamLevel(this, TargetLevelName, false, false, LatentInfo);
		}
	}
}

void ALRTransitionGameMode::OnLevelPreloaded()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->FinishLoading();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			UGameplayStatics::OpenLevel(this, TargetLevelName);
		},
		1.f,
		false
	);
}
