// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LRGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "System/LoggingSystem.h"
#include "UI/Intro/LRLoadingWidget.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"


void ULRGameInstance::OpenNextStage(FName StageID)
{
	SetNextLevelName(ELevelName::Stage);
	SetNextStageID(StageID);
	
	if (UStageManagerSubsystem* StageManager = GetSubsystem<UStageManagerSubsystem>())
	{
		StageManager->LoadStage(StageID);
	}

	OpenNextLevelLatent();
}

void ULRGameInstance::OpenNextLevel()
{
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

void ULRGameInstance::OpenNextLevelLatent()
{
	UUIManagerSubsystem* UIManager = GetSubsystem<UUIManagerSubsystem>();
	UIManager->CloseAllPopupUI();

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
