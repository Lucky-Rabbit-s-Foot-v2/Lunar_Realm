// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/Common/LRGameAbilitySystemMacros.h"
#include "LRPlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ULRPlayerAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_Aether)
	FGameplayAttributeData Aether;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, Aether);

	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(ULRPlayerAttributeSet, AttackPower);

	UFUNCTION() 
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION() 
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION() 
	void OnRep_Aether(const FGameplayAttributeData& OldAether);
	UFUNCTION() 
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
};
