// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_BasicAttack.h"
#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"


ULRGA_BasicAttack::ULRGA_BasicAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void ULRGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ALRCharacter* OwnerChar = Cast<ALRCharacter>(ActorInfo->AvatarActor.Get());
	if (!OwnerChar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* TargetActor = nullptr;
	ULRCombatComponent* CombatComp = OwnerChar->FindComponentByClass<ULRCombatComponent>();
	if (CombatComp)
	{
		TargetActor = CombatComp->GetCurrentTarget();
	}

	if (!TargetActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (DamageEffectClass && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectContextHandle Context = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			if (TargetASC)
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}

	if (AttackMontage)
	{
		OwnerChar->PlayAnimMontage(AttackMontage);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
