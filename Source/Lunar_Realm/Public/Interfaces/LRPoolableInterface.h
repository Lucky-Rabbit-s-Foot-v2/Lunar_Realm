// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LRPoolableInterface.generated.h"

UINTERFACE(MinimalAPI)
class ULRPoolableInterface : public UInterface
{
	GENERATED_BODY()
};


/**
 * LRPoolableInterface 구성 요소
 * - 풀링 할당
 * - 풀링 반환
 */
 //============================================================================
 // (260127) PJB 제작. 제반 사항 구현.
 //============================================================================

class LUNAR_REALM_API ILRPoolableInterface
{
	GENERATED_BODY()

public:
	virtual void OnPoolActivate() = 0;
	virtual void OnPoolDeactivate() = 0;
};
