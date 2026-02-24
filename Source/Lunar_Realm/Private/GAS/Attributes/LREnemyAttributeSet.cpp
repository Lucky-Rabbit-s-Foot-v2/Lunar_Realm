// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyCharacter.h"

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
				LR_INFO(TEXT("[%s] Health reached 0, calling OnDie()"), *EnemyChar->GetName());
				EnemyChar->OnDie();
			}
		}
	}
}
