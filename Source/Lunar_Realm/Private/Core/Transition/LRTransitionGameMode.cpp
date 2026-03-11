// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Transition/LRTransitionGameMode.h"

#include "UI/Intro/LRLoadingWidget.h"
#include "Core/LRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Units/OutGame/LRTransitionController.h"

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
			LatentInfo.ExecutionFunction = FName("OnLevelPreloadCompleted");
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = FMath::Rand();

			UGameplayStatics::LoadStreamLevel(this, TargetLevelName, false, false, LatentInfo);
		}
	}
}

void ALRTransitionGameMode::OnLevelPreloadCompleted()
{
	ALRTransitionController* Controller = Cast<ALRTransitionController>(UGameplayStatics::GetPlayerController(this, 0));
	if (Controller)
	{
		Controller->FinishLoading();
	}
	else
	{
		LR_WARN(TEXT("PlayerController is not of type ALRTransitionController in LRTransitionGameMode"));
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			UGameplayStatics::OpenLevel(this, TargetLevelName);
		},
		PreloadDuration,
		false
	);
}
