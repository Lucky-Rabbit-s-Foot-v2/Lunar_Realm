// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "LRTransitionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRTransitionGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void OnLevelPreloaded();

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLoadingWidget> LoadingWidgetClass = nullptr;

private:
	FName TargetLevelName;

	UPROPERTY()
	TObjectPtr<class ULRLoadingWidget> LoadingWidgetInstance = nullptr;
};
