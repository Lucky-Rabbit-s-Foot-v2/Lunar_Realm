// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures/Core/LRCore.h"
#include "LREnemyCore.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260217) BJM 제작. 적 코어 클래스
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyCore : public ALRCore
{
	GENERATED_BODY()
	
public:
	ALREnemyCore();

protected:
	virtual void BeginPlay() override;

	virtual void OnCoreDestroyed() override;
};
