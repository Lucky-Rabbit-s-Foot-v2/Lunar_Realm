// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LROptionSaveGame.generated.h"


/**
 * 설정 정보 저장을 담당하는 서브시스템
 *
 * 주요 기능:
 * - 사운드 설정 저장 및 불러올 데이터 관리
 */

 //============================================================================
 // (260204) PJB 제작. 제반 사항 구현.
 //============================================================================


UCLASS()
class LUNAR_REALM_API ULROptionSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float MasterVolume = 1.f;

	UPROPERTY(EditAnywhere)
	float BGMVolume = 1.f;

	UPROPERTY(EditAnywhere)
	float SFXVolume = 1.f;

};
