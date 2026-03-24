// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "LRCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void ToggleStages();

	UFUNCTION(Exec)
	void UnlockStages();

	UFUNCTION(Exec)
	void LockStages();

	UFUNCTION(Exec)
	void GameClear();

	UFUNCTION(Exec)
	void GameOver();

	UFUNCTION(Exec)
	void ShowMeTheMoney();

	UFUNCTION(Exec)
	void AddGold(int32 Amount);

	UFUNCTION(Exec)
	void AddCrescentTicket(int32 Amount);

	UFUNCTION(Exec)
	void AddFullMoonTicket(int32 Amount);

	UFUNCTION(Exec)
	void ClearStage();


};
