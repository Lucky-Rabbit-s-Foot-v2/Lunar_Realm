// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_MaidSkill.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "TimerManager.h"
#include "Engine/World.h"

ULRGA_MaidSkill::ULRGA_MaidSkill()
{
	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Maid;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_MAID";
	SkillEffectID = "EFFECT_MAID";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Maid.Cooldown")));

	// 스킬중에 평타 안나가게 막기
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
}

void ULRGA_MaidSkill::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	Super::OnAbilityActivated(InHandle, InActorInfo, InActivationInfo);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SecondStrikeTimer, this, &ULRGA_MaidSkill::PerformStrikeLogic, 0.8f, false);

		World->GetTimerManager().SetTimer(ThirdStrikeTimer, this, &ULRGA_MaidSkill::PerformStrikeLogic, 1.0f, false);
	}
}

void ULRGA_MaidSkill::EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SecondStrikeTimer);
		World->GetTimerManager().ClearTimer(ThirdStrikeTimer);
	}

	Super::EndAbility(InHandle, InActorInfo, InActivationInfo, bInReplicateEndAbility, bInWasCancelled);
}
