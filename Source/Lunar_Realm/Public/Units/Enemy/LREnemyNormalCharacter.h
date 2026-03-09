// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "LREnemyNormalCharacter.generated.h"

/**
 * LREnemyNormalCharacter 구성 요소
 * 잡몹 적 캐릭터 클래스 (기지로 돌진 and 유효 범위 내에 Player or Member 존재 시 공격)
 * - LRAIController
 * - LREnemyAttributeSet
 * - 
 */
 //============================================================================
 // (260204) KWB 제작.
 // (260305) KWB 에너미 스폰, 사망 시 태그 관리를 위한 로직 추가
 //============================================================================
UCLASS()
class LUNAR_REALM_API ALREnemyNormalCharacter : public ALREnemyCharacter
{
	GENERATED_BODY()
	
public:
	ALREnemyNormalCharacter();

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;
};
