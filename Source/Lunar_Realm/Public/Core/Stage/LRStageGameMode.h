// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "TimerManager.h"
#include "LRStageGameMode.generated.h"

// =============================================================================
/**
 * InGame 전용 게임모드
 */
//=============================================================================
// (260203) PJB 제작.
// (260223) BJM 승패판정 로직 추가
// (260226) PJB 일시정지, 승패판정 UI 연동 추가
// (260322) KWB 게임 시작 알리는 델리게이트, 스테이지 초기화, 게임 시작 추가
// (260327) KWB 타이머 기반 시작 로직 삭제, LRReadyStartWidget 기반 시작 로직으로 변경
//			    LRReadyStartWidget 위젯 애니메이션 재생 중 터치, 키보드, 마우스 입력 방지
// (260331) KWB 코어 파괴 시 스포너 정지 신호를 전달하기 위한 델리게이트(FOnGameEnding), 콜백 함수 추가
//=============================================================================

class USoundBase;
class UAudioComponent;
class ULRReadyStartWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameEnding);

UCLASS()
class LUNAR_REALM_API ALRStageGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* InPlayer) override;

protected:
	virtual void BeginPlay() override;

	void OnInitializeStage();
	
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void OnBGMFinished();

	UFUNCTION()
	void OnReadySequenceFinished();

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
	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnBossAppearance();
	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnPlayReadyStartAnimation();


	void PlayBGM();

	void CleanupUnusedCores();

private:
	bool IsStar1ConditionCheck();
	bool IsStar2ConditionCheck();
	bool IsStar3ConditionCheck();

	int32 CalculateStarMasking();
	void OpenGameClearPopupWidget(int32 InStarMasking);

	UFUNCTION()
	void OnAnyCoreDestroyed(AActor* DestroyedCore);

private:
	bool bIsGamePause = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TArray<TObjectPtr<USoundBase>> BGMList;
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GameLogic")
	FOnGameStarted OnGameStarted;

	UPROPERTY(BlueprintAssignable, Category = "GameLogic")
	FOnGameEnding OnGameEnding;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Stage")
	int32 PlayerDeathCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Stage")
	float GameStartDelay = 0.5f;

	FTimerHandle GameStartDelayTimer;

public:
	void AddPlayerDeathCount();

};
