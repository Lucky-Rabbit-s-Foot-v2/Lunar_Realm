// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

ULRPlayerAttributeSet::ULRPlayerAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitAether(0.0f);
	InitAttackPower(10.0f);
}

void ULRPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULRPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULRPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULRPlayerAttributeSet, Aether, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULRPlayerAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
}

void ULRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void ULRPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Die"));
		}
	}
}

void ULRPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(ULRPlayerAttributeSet, Health, OldHealth); }
void ULRPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(ULRPlayerAttributeSet, MaxHealth, OldMaxHealth); }
void ULRPlayerAttributeSet::OnRep_Aether(const FGameplayAttributeData& OldAether) { GAMEPLAYATTRIBUTE_REPNOTIFY(ULRPlayerAttributeSet, Aether, OldAether); }
void ULRPlayerAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) { GAMEPLAYATTRIBUTE_REPNOTIFY(ULRPlayerAttributeSet, AttackPower, OldAttackPower); }
