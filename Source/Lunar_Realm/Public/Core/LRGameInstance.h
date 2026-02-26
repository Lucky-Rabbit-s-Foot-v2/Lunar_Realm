// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Subsystems/Settings/MapSettings.h"
#include "LRGameInstance.generated.h"

// =============================================================================
/**
 * 프로젝트 게임 인스턴스
 */
//=============================================================================
// (260206) PJB 제작. 비동기 레벨 전환 기능 구현
// (260208) KWB Stage 관리 주체 GI로 가정 -> 관련 내용 해당 클래스에 작성 (임시)
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void OpenNextStage(FName StageID);

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void OpenNextLevelByName(ELevelName LevelName);

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void OpenNextLevel();

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	FName GetNextLevelName() const { return NextLevelName; }

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void SetNextLevelName(ELevelName LevelName);

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	FName GetNextStageID() const { return NextStageID; }

	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void SetNextStageID(FName StageID) { NextStageID = StageID; }

private:
	void OpenNextLevelLatent();

protected:
	FName NextLevelName = NAME_None;
	FName NextStageID = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLoadingWidget> LoadingWidgetClass;

public:
	UPROPERTY()
	TObjectPtr<class ULRLoadingWidget> LoadingWidgetInstance = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	void SetCurrentStageID(FName InStageID) { CurrentStageID = InStageID; }

	UFUNCTION(BlueprintPure, Category = "LR|Stage")
	FName GetCurrentStageID() const { return CurrentStageID; }

private:
	UPROPERTY(EditAnywhere, Category = "LR|Stage")
	FName CurrentStageID;
};
