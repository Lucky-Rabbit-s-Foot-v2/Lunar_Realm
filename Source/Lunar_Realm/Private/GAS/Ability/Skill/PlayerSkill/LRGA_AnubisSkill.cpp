// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_AnubisSkill.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_AnubisSkill::ULRGA_AnubisSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Anubis;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_ANUBIS";
	SkillEffectID = "EFFECT_ANUBIS";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Anubis.Cooldown")));
}
