// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "LRStageGameMode.generated.h"

// =============================================================================
/**
 * InGame 전용 게임모드
 */
//=============================================================================
// (260203) PJB 제작.
// (260223) BJM 승패판정 로직 추가
// (260226) PJB 일시정지, 승패판정 UI 연동 추가
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALRStageGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	// TODO: 게임 멈추는 로직 필요

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnGameOver();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnGameClear();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnRestartGame();
	
	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnPauseGame();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnOpenPauseUI();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnResumeGame();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnResetStage();

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnExitStage();
};
