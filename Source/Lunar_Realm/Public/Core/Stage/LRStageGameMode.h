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

class USoundBase;
class UAudioComponent;

UCLASS()
class LUNAR_REALM_API ALRStageGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* InPlayer) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBGMFinished();
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
	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnStartNextStage();
	void PlayBGM();

	void CleanupUnusedCores();

private:
	bool IsStar1ConditionCheck();
	bool IsStar2ConditionCheck();
	bool IsStar3ConditionCheck();

	int32 CalculateStarMasking();
	void OpenGameClearPopupWidget(int32 InStarMasking);


private:
	bool bIsGamePause = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TArray<TObjectPtr<USoundBase>> BGMList;
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;
	
};
