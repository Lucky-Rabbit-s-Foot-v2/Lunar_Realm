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
// (260303) KWB 에테르 관련 멤버 및 델리게이트 추가.
//=============================================================================

DECLARE_DELEGATE_OneParam(FOnAetherChangedDelegate, float);

UCLASS()
class LUNAR_REALM_API ALRStageGameState : public ALRGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	// 에테르 추가 함수 (Enemy가 호출)
	void AddAether(float Amount);

	// Player가 바인딩할 델리게이트
	FOnAetherChangedDelegate OnAetherChanged;

private:
	// 현재 보유 에테르 (향후 SpawnManager가 사용)
	float CurrentAether = 0.0f;
};
