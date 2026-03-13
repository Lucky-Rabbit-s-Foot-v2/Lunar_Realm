// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "LRTransitionGameMode.generated.h"

// =============================================================================
/**
 * 비동기 레벨 전환용 게임모드
 */
//=============================================================================
// (260205) PJB 제작.
// (260219) PJB 수정. HUD 제거, 로딩 화면 소스 코드 이관.
//=============================================================================

UCLASS()
class LUNAR_REALM_API ALRTransitionGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnLevelPreloadCompleted();

protected:
	UPROPERTY(EditAnywhere, Category = "LR|Loading|Preload")
	float PreloadDuration = 3.f;

private:
	FName TargetLevelName = NAME_None;
};
