// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"

ULREnemyAttributeSet::ULREnemyAttributeSet()
{
}

void ULREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxAttack);
	}

	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxSpeed);
	}

}

void ULREnemyAttributeSet::OnDamageExecuted(float InDamageDone, const FGameplayEffectModCallbackData& Data)
{
	if (InDamageDone > 0.0f)
	{
		if (ALREnemyCharacter* EnemyChar = Cast<ALREnemyCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
		{
			if (GetHealth() > 0.0f)
			{
				EnemyChar->PlayAttackedMontage();
			}
			else
			{
				EnemyChar->OnDie();
			}
		}
	}
}
