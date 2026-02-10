// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"

ULREnemyAttributeSet::ULREnemyAttributeSet()
{
	// Properties Init Val
}

void ULREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		// NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}
