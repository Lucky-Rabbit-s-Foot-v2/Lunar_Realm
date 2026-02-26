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
	SetNextLevelName(ELevelName::STAGE);
	SetNextStageID(StageID);
	
	if (UStageManagerSubsystem* StageManager = GetSubsystem<UStageManagerSubsystem>())
	{
		StageManager->LoadStage(StageID);
	}

	OpenNextLevelLatent();
}

void ULRGameInstance::OpenNextLevelByName(ELevelName LevelName)
{
	SetNextLevelName(LevelName);
	OpenNextLevelLatent();
}

void ULRGameInstance::OpenNextLevel()
{
	OpenNextLevelLatent();
}

void ULRGameInstance::SetNextLevelName(ELevelName LevelName)
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	if (!MapSettings)
	{
		return;
	}

	if (const TSoftObjectPtr<UWorld>* TargetMapPtr = MapSettings->LevelMap.Find(LevelName))
	{
		NextLevelName = FName(TargetMapPtr->GetLongPackageFName());
	}
	else
	{
		LR_ERROR(TEXT("LevelName %d not found in MapSettings LevelMap"), static_cast<uint8>(LevelName));
	}
}

void ULRGameInstance::OpenNextLevelLatent()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			const UMapSettings* MapSettings = GetDefault<UMapSettings>();
			const TSoftObjectPtr<UWorld>* TransitionMap = MapSettings->LevelMap.Find(ELevelName::TRANSITION);
			
			if (UUIManagerSubsystem* UIManager = GetSubsystem<UUIManagerSubsystem>())
			{
				UIManager->ResetAllUIStates();
			}

			if (TransitionMap)
			{
				UGameplayStatics::OpenLevel(this, FName(TransitionMap->GetLongPackageName()));
			}
			else
			{
				LR_ERROR(TEXT("TransitionMap not set in MapSettings. Opening level %s directly."), *NextLevelName.ToString());
			}
		},
		0.1f,
		false
	);
}
