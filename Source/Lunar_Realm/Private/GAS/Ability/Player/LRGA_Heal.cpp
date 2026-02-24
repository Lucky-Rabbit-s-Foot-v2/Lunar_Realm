// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_Heal.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Tags/LRGameplayTags.h"


ULRGA_Heal::ULRGA_Heal()
{
	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal")));
	// FGameplayTagContainer Tags;
	// Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal")));
	// SetAssetTags(Tags);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Skill_Heal);
	SetAssetTags(TempTags);
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Heal;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void ULRGA_Heal::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (HealEffectClass && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayEffectContextHandle ContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(HealEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("체력 회복 성공!"));
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool ULRGA_Heal::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return false;

	float CurrentHealth = ASC->GetNumericAttribute(ULRPlayerAttributeSet::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttribute(ULRPlayerAttributeSet::GetMaxHealthAttribute());

	if (CurrentHealth >= MaxHealth)
	{
		UE_LOG(LogTemp, Warning, TEXT("체력이 이미 가득 참"));
		return false; 
	}

	return true;
}
