// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LRIntroController.generated.h"


// =============================================================================
/**
 * 인트로 전용 컨트롤러
 */
 //=============================================================================
 // (260219) PJB 제작. IntroHUD 에서 코드 이관
 // (260219) PJB 수정. 세션 추가
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ALRIntroController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	virtual void OpenFirstWidget() override;
};
