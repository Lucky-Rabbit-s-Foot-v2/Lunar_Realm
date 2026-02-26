// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

void ALRStageGameMode::OnGameOver()
{
	LR_SCREEN_INFO(TEXT("Game Over : Not implemented yet"));
}

void ALRStageGameMode::OnGameClear()
{
	LR_SCREEN_INFO(TEXT("Game Clear : Not implemented yet"));
}

void ALRStageGameMode::OnRestartGame()
{
	OnResetStage();
	LR_SCREEN_INFO(TEXT("Restart : Not implemented yet"));
}

void ALRStageGameMode::OnPauseGame()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ALRStageGameMode::OnOpenPauseUI()
{
	OnPauseGame();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::PAUSE);
}

void ALRStageGameMode::OnResumeGame()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ALRStageGameMode::OnResetStage()
{
	LR_SCREEN_INFO(TEXT("Reset : Not implemented yet"));
}

void ALRStageGameMode::OnExitStage()
{
	OnResumeGame();

	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextLevelByName(ELevelName::LOBBY);
}
