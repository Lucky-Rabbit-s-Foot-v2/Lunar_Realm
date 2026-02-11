// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"

ULREnemyAttributeSet::ULREnemyAttributeSet()
{
	// TEMP : 추후 실제 값으로 변경 필요
	InitHealth(10.0f);
	InitAttack(20.0f);
	InitSpeed(150.0f);
}

void ULREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth);
	}

	if (Attribute == GetAttackAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxAttack);
	}

	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxSpeed);
	}
}

void ULREnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.0f)
		{
			// LR_DEBUG(TEXT("%s "));
		}
	}
}
