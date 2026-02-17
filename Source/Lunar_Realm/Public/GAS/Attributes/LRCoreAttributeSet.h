// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "GAS/Common/LRGameAbilitySystemMacros.h"
#include "AbilitySystemComponent.h"

#include "LRCoreAttributeSet.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260217) BJM 제작. 코어 전용 어트리뷰트셋
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRCoreAttributeSet : public ULRAttributeSet
{
	GENERATED_BODY()
	
public:
	ULRCoreAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULRCoreAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULRCoreAttributeSet, MaxHealth);

};
