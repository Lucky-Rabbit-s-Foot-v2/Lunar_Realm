// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "LROutGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALROutGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
public:
	ALROutGameMode();

protected:
	virtual void BeginPlay() override;

};
