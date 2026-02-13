// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Transition/LRTransitionGameMode.h"

#include "UI/Intro/LRLoadingWidget.h"
#include "Core/LRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Subsystems/UIManagerSubsystem.h"

void ALRTransitionGameMode::BeginPlay()
{
	Super::BeginPlay();

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
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (LoadingWidgetClass)
	{
		ULRLoadingWidget* LoadingWidgetInstance = UIManager->GetOrCreateWidget<ULRLoadingWidget>(LoadingWidgetClass);
		LoadingWidgetInstance->FinishLoading();
	}
	else
	{
		LR_WARN(TEXT("LoadingWidgetClass is not set in LRTransitionGameMode"));
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
