// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_NobleSkill.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_NobleSkill::ULRGA_NobleSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Noble;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_NOBLE";
	SkillEffectID = "EFFECT_NOBLE";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Noble.Cooldown")));
}
