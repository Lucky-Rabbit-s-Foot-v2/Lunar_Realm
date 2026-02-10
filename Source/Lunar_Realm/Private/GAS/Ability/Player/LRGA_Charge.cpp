// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_Charge.h"
#include "AbilitySystemComponent.h"

ULRGA_Charge::ULRGA_Charge()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Aether.Charge")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void ULRGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ChargeEffectClass && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayEffectContextHandle ContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(ChargeEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			UE_LOG(LogTemp, Log, TEXT("에테르 충전"));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
}
