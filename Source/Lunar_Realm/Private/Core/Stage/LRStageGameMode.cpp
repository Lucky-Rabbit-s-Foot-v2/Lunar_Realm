// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

void ALRStageGameMode::OnGameOver()
{
	// 주의사항: UI 애니메이션도 멈춤
	// TODO : 방법 고민 필요. 일단은 일시정지 로직 재활용
	OnPauseGame();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::GAMEOVER);
}

void ALRStageGameMode::OnGameClear()
{
	// 주의사항: UI 애니메이션도 멈춤
	// TODO : 방법 고민 필요. 일단은 일시정지 로직 재활용
	OnPauseGame();

	// TODO: 보상 반영 등 코드 추가 필요하면 여기에 작성
	
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::GAMECLEAR);
}

void ALRStageGameMode::OnRestartGame()
{
	OnResetStage();
	LR_SCREEN_INFO(TEXT("Restart : Not implemented yet"));
}

void ALRStageGameMode::OnPauseGame()
{
	if (bIsGamePause)
	{
		return;
	}
	bIsGamePause = true;
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
	if(!bIsGamePause)
	{
		return;
	}
	bIsGamePause = false;
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

void ALRStageGameMode::OnStartNextStage()
{
	LR_SCREEN_INFO(TEXT("Start Next Stage : Not implemented yet"));
}
