// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameStateBase.h"
#include "LRStageGameState.generated.h"

// =============================================================================
/**
 * InGame 전용 게임상태
 */
//=============================================================================
// (260203) PJB 제작.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALRStageGameState : public ALRGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
