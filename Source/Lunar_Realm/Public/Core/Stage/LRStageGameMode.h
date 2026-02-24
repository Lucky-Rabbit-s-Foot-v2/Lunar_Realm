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
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALRStageGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void OnGameOver(bool bInPlayerWon);

protected:
	bool bIsGameOver = false;

};
