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
 * - 오브젝트 풀링 인터페이스
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
	/* 풀 할당할 때 진행할 로직
	* - 시각적/물리적 상태 복구
	* - 데이터를 할당 받아서 스텟 등 복구
	* - 움직임 멈춤 해제
	* - 빙의 등
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolActivate();


	/* 풀 반환할 때 진행할 로직
	* - 모든 동작 정지
	* - 타이머 정리
	* - GAS 효과 제거 등
	* - 빙의 해제
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolDeactivate();
};
