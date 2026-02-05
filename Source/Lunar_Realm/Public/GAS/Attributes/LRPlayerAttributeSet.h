// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/Common/LRGameAbilitySystemMacros.h"
#include "LRPlayerAttributeSet.generated.h"

/**
 * 
 */

//=============================================================================
// (260203) BJM 제작. 플레이어 캐릭터 어트리뷰트셋.
// (260205) 멀티플레이 코드 제거 및 방어력(Defense) 추가 완료_BJM
//=============================================================================
UCLASS()
class LUNAR_REALM_API ULRPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ULRPlayerAttributeSet();

	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Aether;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, Aether);

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, AttackPower);

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, Defense);



};
