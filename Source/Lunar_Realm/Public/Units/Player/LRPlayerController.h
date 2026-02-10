// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LRPlayerController.generated.h"

/**
 * 
 */

//=============================================================================
// (260203) BJM 제작. 플레이어 컨트롤러
// (260210) BJM 카메라매니저 연동
//=============================================================================

class UTouchInterface;

UCLASS()
class LUNAR_REALM_API ALRPlayerController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	ALRPlayerController();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Mobile")
	TObjectPtr<UTouchInterface> MobileTouchInterface;
};
