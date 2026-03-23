// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "LRPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBounds")
	float StageMinY = -2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBounds")
	float StageMaxY = 2000.0f;

};
