// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LRGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "System/LoggingSystem.h"
#include "UI/Intro/LRLoadingWidget.h"

void ULRGameInstance::ChangeLevelAsync(FName LevelToLoad)
{
	LR_SCREEN_INFO(TEXT("Current Level: %s"), *CurrentLevelName.ToString());
	LR_SCREEN_INFO(TEXT("Changing Level Asynchronously to: %s"), *LevelToLoad.ToString());

	if (!CurrentLevelName.IsNone())
	{
		if (LoadingWidgetClass)
		{
			LoadingWidgetInstance = CreateWidget<ULRLoadingWidget>(this, LoadingWidgetClass);

			if (LoadingWidgetInstance)
			{
				LoadingWidgetInstance->AddToViewport(1000);
			}
		}

		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;

		UGameplayStatics::UnloadStreamLevel(this, CurrentLevelName, LatentInfo, false);
	}
	
	FLatentActionInfo LoadLatentInfo;
	LoadLatentInfo.CallbackTarget = this;
	LoadLatentInfo.ExecutionFunction = FName("OnLevelLoadComplete");
	LoadLatentInfo.Linkage = 0;
	LoadLatentInfo.UUID = FMath::Rand();

	UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, false, LoadLatentInfo);
	
	CurrentLevelName = LevelToLoad;
}

void ULRGameInstance::OnLevelLoadComplete()
{
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(
		WaitHandle,
		this,
		&ULRGameInstance::RemoveLoadingScreen,
		3.0f, // 3초 지연
		false
	);

	LR_SCREEN_INFO(TEXT("Level Load Complete: %s"), *CurrentLevelName.ToString());
}

void ULRGameInstance::RemoveLoadingScreen()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->RemoveFromParent();
		LoadingWidgetInstance = nullptr;
	}
	LR_SCREEN_INFO(TEXT("Loading Screen Removed!"));
}
