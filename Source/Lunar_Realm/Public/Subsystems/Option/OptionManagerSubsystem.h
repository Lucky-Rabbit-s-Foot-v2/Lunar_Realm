// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/LROptionSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OptionManagerSubsystem.generated.h"

//============================================================================
/**
 * 옵션 관련 담당 서브시스템
 * - 옵션 카테고리별 관리
 */
 //============================================================================
 // (260210) PJB 제작. 제반 사항 구현.
 //============================================================================

UCLASS()
class LUNAR_REALM_API UOptionManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void LoadOptions();
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void SaveOptions();

	//////////////////////////////////////////////
	// Option Data Getter
	//////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	ULROptionSaveGame* GetCurrentOptionSaveGame() { return CurrentOptionSaveGame; }

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentOptionSaveGame();

private:
	/**
	* 저장된 옵션 데이터가 없을 경우 새로 생성.
	*/
	void CreateNewOptionSaveData();
	
	void InitializeOptions();

private:
	UPROPERTY()
	TObjectPtr<ULROptionSaveGame> CurrentOptionSaveGame;

	const FString SaveSlotName = TEXT("GlobalOptionData");
	const uint32 UserIndex = 0;
};
