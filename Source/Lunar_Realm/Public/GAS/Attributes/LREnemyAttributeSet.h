// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/Common/LRGameAbilitySystemMacros.h"
#include "LREnemyAttributeSet.generated.h"

/**
 * LREnemyAttributeSet 구성 요소
 * Enemy의 기본 AttributeSet
 * - 체력, 공격력
 * - 적 다수 생성시 연산 버거울 수 있으니 최대한 요소 추가 자제
 */
 //============================================================================
 // (260204) KWB 제작.
 // TODO 하신 형님 보여주신 예시대로 데이터 드리븐 구조로 설정
 //============================================================================
UCLASS()
class LUNAR_REALM_API ULREnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	

public:
	ULREnemyAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULREnemyAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(ULREnemyAttributeSet, Attack)

		virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
