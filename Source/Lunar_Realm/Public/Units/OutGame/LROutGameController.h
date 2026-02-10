// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LROutGameController.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260210) BJM 제작. 아웃게임 컨트롤러 제작
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ALROutGameController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	// 콘솔 명령어로 캐릭터나 장비가 활률대로 나오는지 확인하는 함수
	UFUNCTION(Exec)
	void GachaSim(const FString& BannerIdStr, int32 TotalPulls = 100000, int32 Seed = 12345);
};
