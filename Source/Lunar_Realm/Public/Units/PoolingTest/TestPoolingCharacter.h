// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "TestPoolingCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ATestPoolingCharacter : public ALRCharacter
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

};
