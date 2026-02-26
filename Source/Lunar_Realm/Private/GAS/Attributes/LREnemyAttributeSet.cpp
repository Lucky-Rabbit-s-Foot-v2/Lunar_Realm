// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyCharacter.h"

ULREnemyAttributeSet::ULREnemyAttributeSet()
{
}

void ULREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
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
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (GetHealth() <= 0.0f)
		{
			UAbilitySystemComponent* ASC = Data.Target.AbilityActorInfo.Get()->AbilitySystemComponent.Get();
			if (!ASC)
			{
				return;
			}

			AActor* OwnerActor = ASC->GetOwnerActor();

			if (ALREnemyCharacter* EnemyChar = Cast<ALREnemyCharacter>(OwnerActor))
			{
				EnemyChar->OnDie();
			}
		}
	}
}
