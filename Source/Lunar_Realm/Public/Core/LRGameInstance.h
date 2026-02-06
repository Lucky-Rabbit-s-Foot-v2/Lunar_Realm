// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LRGameInstance.generated.h"

/**
 * (260206 PJB) 비동기 레벨 전환 기능 구현
 */

UENUM(BlueprintType)
enum class ELevelName : uint8
{
	None			UMETA(DisplayName = "None"),
	Transition		UMETA(DisplayName = "Transition"),
	Intro			UMETA(DisplayName = "Intro"),
	Lobby			UMETA(DisplayName = "Lobby"),
	Stage			UMETA(DisplayName = "Stage")
};

UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void OpenNextLevel();

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	FName GetNextLevelName() const { return NextLevelName; }

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void SetNextLevelName(ELevelName LevelName);

private:
	void ShowLoadingWidget();
	void OpenNextLevelLatent();

protected:
	FName NextLevelName = EName::None;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLoadingWidget> LoadingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Level Streaming")
	TSoftObjectPtr<UWorld> Map_Transition;

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
