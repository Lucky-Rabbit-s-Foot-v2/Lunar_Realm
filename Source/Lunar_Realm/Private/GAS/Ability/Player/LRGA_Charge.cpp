// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_Charge.h"
#include "AbilitySystemComponent.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_Charge::ULRGA_Charge()
{
	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Aether.Charge")));

	// FGameplayTagContainer Tags;
	// Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Aether.Charge")));
	// SetAssetTags(Tags);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Aether_Charge);
	SetAssetTags(TempTags);
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Aether_Charge;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}


void ULRGA_Charge::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
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
