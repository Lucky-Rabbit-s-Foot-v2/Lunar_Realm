// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LRGameModeBase.generated.h"

// =============================================================================
/**
 * 프로젝트 최상위 게임모드
 */
//=============================================================================
// (260203) PJB 제작.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;	
};
