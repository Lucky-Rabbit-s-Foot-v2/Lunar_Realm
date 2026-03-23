// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_EnchanterSkill.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_EnchanterSkill::ULRGA_EnchanterSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Enchanter;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_ENCHANTER";
	SkillEffectID = "EFFECT_ENCHANTER";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Enchanter.Cooldown")));

}
