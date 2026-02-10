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
 // (260210) KWB 멤버 추가(속성, 속성 한계값), 
 //============================================================================
UCLASS()
class LUNAR_REALM_API ULREnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	

public:
	ULREnemyAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "LR|Spec")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULREnemyAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "LR|Spec")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(ULREnemyAttributeSet, Attack)

	UPROPERTY(BlueprintReadOnly, Category = "LR|Spec")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(ULREnemyAttributeSet, Speed)

	// 속성값 한계치
	UPROPERTY(BlueprintReadWrite, Category = "LR|Spec|Limits")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "LR|Spec|Limits")
	float MaxSpeed = 2.0f; // 1.0 = 100%, 2.0 = 200%

	UPROPERTY(BlueprintReadWrite, Category = "LR|Spec|Limits")
	float MaxAttack = 3.0f; // 1.0 = 100%, 3.0 = 300%

	UPROPERTY(BlueprintReadWrite, Category = "LR|Spec|Limits")
	float MaxAttackRange = 2000.0f;	// TEMP : 임시값 엔진에서 거리 보면서 조정 필요

	UPROPERTY(BlueprintReadWrite, Category = "LR|Spec|Limits")
	float MaxScale = 1.5f;	// 150%

		virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
