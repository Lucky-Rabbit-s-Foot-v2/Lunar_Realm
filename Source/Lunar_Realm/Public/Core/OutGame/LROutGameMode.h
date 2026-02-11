// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "LROutGameMode.generated.h"

// =============================================================================
/**
 * OutGame 전용 게임모드
 */
//=============================================================================
// (260203) PJB 제작.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALROutGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	ALROutGameMode();

protected:
	virtual void BeginPlay() override;

};
