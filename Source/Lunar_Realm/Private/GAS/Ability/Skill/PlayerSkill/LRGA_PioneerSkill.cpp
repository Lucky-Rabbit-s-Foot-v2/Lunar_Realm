// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_PioneerSkill.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "TimerManager.h"
#include "Engine/World.h"

ULRGA_PioneerSkill::ULRGA_PioneerSkill()
{
	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Pioneer;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_PIONEER";
	SkillEffectID = "EFFECT_PIONEER";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Pioneer.Cooldown")));

	// 스킬중에 평타 안나가게 막기
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
}

void ULRGA_PioneerSkill::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	Super::OnAbilityActivated(InHandle, InActorInfo, InActivationInfo);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SecondStrikeTimer, this, &ULRGA_PioneerSkill::PerformStrikeLogic, 0.7f, false);

		World->GetTimerManager().SetTimer(SecondStrikeTimer, this, &ULRGA_PioneerSkill::PerformStrikeLogic, 1.3f, false);

	}
}

void ULRGA_PioneerSkill::EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SecondStrikeTimer);
		World->GetTimerManager().ClearTimer(ThirdStrikeTimer);
	}

	Super::EndAbility(InHandle, InActorInfo, InActivationInfo, bInReplicateEndAbility, bInWasCancelled);
}
