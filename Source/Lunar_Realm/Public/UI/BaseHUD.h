// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"


// =============================================================================
/**
 * UI HUD의 베이스 클래스
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
