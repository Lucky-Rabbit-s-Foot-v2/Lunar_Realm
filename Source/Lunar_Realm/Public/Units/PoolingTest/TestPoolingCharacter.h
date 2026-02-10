// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "TestPoolingCharacter.generated.h"

//============================================================================
/**
 * 오브젝트 풀링 테스트용 캐릭터
 */
 //============================================================================
 // (260203) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ATestPoolingCharacter : public ALRCharacter
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

};
