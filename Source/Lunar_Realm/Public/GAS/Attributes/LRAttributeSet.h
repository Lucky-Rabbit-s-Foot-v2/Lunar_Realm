// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/Common/LRGameAbilitySystemMacros.h"
#include "LRAttributeSet.generated.h"

// =============================================================================
/**
 * ULRAttributeSet 구성 요소
 * - 캐릭터, 에너미 공통 속성
 * - 모든 AttributeSet의 베이스
 * - TODO: 캐릭터, 에너미 공통 속성 합의 및 추가
 */
 //=============================================================================
 // (260210) KWB 제작. 제반 사항 구현.
 // (260224) KHS 수정, 공통 어트리뷰트 추가
 // (260303) BJM 수정, 데미치 항목 추가
 // (260306) BJM 수정, 공격력, 방어력 추가
 // (260310) BJM 수정, 자식 클래스에서 겹치는 함수 통합
 // =============================================================================
UCLASS()
class LUNAR_REALM_API ULRAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Player/Enemy/Core 모두 공통으로 사용하는 어트리뷰트
	UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, MaxHealth)
	
	UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, Speed)

	UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, Damage)
	
	UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, AttackPower)

		UPROPERTY(BlueprintReadOnly, Category = "LR|Common")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(ULRAttributeSet, Defense)

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	virtual FLinearColor GetDamageTextColor(float InDamage) const
	{
		return (InDamage < 0.0f) ? FLinearColor::Green : FLinearColor::White;
	}

	virtual void OnDamageExecuted(float InDamageDone, const FGameplayEffectModCallbackData& Data) {}

};
