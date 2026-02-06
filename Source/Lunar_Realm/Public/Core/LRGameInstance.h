// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LRGameInstance.generated.h"

/**
 * (260206 PJB) 비동기 레벨 전환 기능 구현
 */
UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	/**
	* 레벨 이동할 때 사용하는 비동기 함수
	* @param LevelToLoad	로드할 레벨 이름
	*/
	UFUNCTION(BlueprintCallable, Category = "LR|LevelStreming")
	void ChangeLevelAsync(FName LevelToLoad);

	UFUNCTION(BlueprintCallable, Category = "LR|LevelStreming")
	void GoToIntro();

	UFUNCTION(BlueprintCallable, Category = "LR|LevelStreming")
	void GoToLobby();

	UFUNCTION(BlueprintCallable, Category = "LR|LevelStreming")
	void GoToStage();

protected:
	/**
	* 레벨 로드 완료 콜백 함수. 비동기 로드가 완료되면 자동으로 호출
	*/
	UFUNCTION()
	void OnLevelLoadComplete();

private:
	void UnloadCurrentLevel();
	void LoadLatentNewLevel(const FName& LevelToLoad);

	void ShowLoadingScreen();
	void CompleteLoadingScreen();
	void RemoveLoadingScreen();

protected:
	FName CurrentLevelName = EName::None;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLoadingWidget> LoadingWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Level Streaming")
	TSoftObjectPtr<UWorld> Map_Intro;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Level Streaming")
	TSoftObjectPtr<UWorld> Map_Lobby;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Level Streaming")
	TSoftObjectPtr<UWorld> Map_Stage;

private:
	UPROPERTY()
	TObjectPtr<class ULRLoadingWidget> LoadingWidgetInstance = nullptr;
};
