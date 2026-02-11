// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LRGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "System/LoggingSystem.h"
#include "UI/Intro/LRLoadingWidget.h"

void ULRGameInstance::OpenNextLevel()
{
	ShowLoadingWidget();
	OpenNextLevelLatent();
}

void ULRGameInstance::SetNextLevelName(ELevelName LevelName)
{
	switch (LevelName)
	{
	case ELevelName::Intro:
		NextLevelName = FName(Map_Intro.GetLongPackageFName());
		break;
	case ELevelName::Stage:
		NextLevelName = FName(Map_Stage.GetLongPackageFName());
		break;
	case ELevelName::Lobby:
		NextLevelName = FName(Map_Lobby.GetLongPackageFName());
		break;
	default:
		LR_SCREEN_INFO(TEXT("Invalid LevelName enum value"));
		break;
	}
}

void ULRGameInstance::ShowLoadingWidget()
{
	if (LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULRLoadingWidget>(GetWorld(), LoadingWidgetClass);
		if (LoadingWidgetInstance)
		{
			LoadingWidgetInstance->AddToViewport(1000);
		}
	}
}

void ULRGameInstance::OpenNextLevelLatent()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			UGameplayStatics::OpenLevel(this, Map_Transition.GetLongPackageFName());
		},
		0.1f,
		false
	);
}
