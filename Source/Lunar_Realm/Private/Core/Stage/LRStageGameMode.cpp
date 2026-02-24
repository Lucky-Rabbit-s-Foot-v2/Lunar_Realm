// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameMode.h"
#include "System/LoggingSystem.h"

void ALRStageGameMode::OnGameOver(bool bInPlayerWon)
{
	if (bIsGameOver)
	{
		return;
	}
	bIsGameOver = true;

	if (bInPlayerWon)
	{
		LR_INFO(TEXT("게임종료 : 플레이어 승리"));
	}
	else
	{
		LR_INFO(TEXT("게임종료 : 플레이어 패배"));
	}
}
