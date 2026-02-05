// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRPlayerController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void GachaSim(const FString& BannerIdStr, int32 TotalPulls = 100000, int32 Seed = 12345);
};
