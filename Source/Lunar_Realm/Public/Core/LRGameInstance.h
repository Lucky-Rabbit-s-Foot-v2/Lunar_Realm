// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LRGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "LR|LevelStreming")
	void ChangeLevelAsync(FName LevelToLoad);

protected:
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

private:
	UPROPERTY()
	TObjectPtr<class ULRLoadingWidget> LoadingWidgetInstance = nullptr;
};
