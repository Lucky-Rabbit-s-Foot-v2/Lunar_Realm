// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LRGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "System/LoggingSystem.h"
#include "UI/Intro/LRLoadingWidget.h"

void ULRGameInstance::ChangeLevelAsync(FName LevelToLoad)
{
	if (!CurrentLevelName.IsNone())
	{
		ShowLoadingScreen();
		UnloadCurrentLevel();
	}
	LoadLatentNewLevel(LevelToLoad);
}

void ULRGameInstance::OnLevelLoadComplete()
{
	CompleteLoadingScreen();

	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(
		WaitHandle,
		this,
		&ULRGameInstance::RemoveLoadingScreen,
		1.0f,
		false
	);
}


void ULRGameInstance::UnloadCurrentLevel()
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	UGameplayStatics::UnloadStreamLevel(this, CurrentLevelName, LatentInfo, false);
}

void ULRGameInstance::LoadLatentNewLevel(const FName& LevelToLoad)
{
	FLatentActionInfo LoadLatentInfo;
	LoadLatentInfo.CallbackTarget = this;
	LoadLatentInfo.ExecutionFunction = FName("OnLevelLoadComplete");
	LoadLatentInfo.Linkage = 0;
	LoadLatentInfo.UUID = FMath::Rand();

	UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, false, LoadLatentInfo);

	CurrentLevelName = LevelToLoad;
}


void ULRGameInstance::ShowLoadingScreen()
{
	if (LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULRLoadingWidget>(this, LoadingWidgetClass);

		if (LoadingWidgetInstance)
		{
			LoadingWidgetInstance->AddToViewport(1000);
		}
	}
}

void ULRGameInstance::CompleteLoadingScreen()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->FinishLoading();
	}
}

void ULRGameInstance::RemoveLoadingScreen()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->RemoveFromParent();
		LoadingWidgetInstance = nullptr;
	}
}
